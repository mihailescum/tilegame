-- Loads content/worlds/world1.world and its maps/tilesets directly from their raw Tiled JSON
-- (via _load_json - the sandboxed Lua state cannot read files itself), and builds the same
-- entities the former systems::Map used to build in C++: one TileLayer entity per tile layer,
-- and one sprite entity per tile object (NPCs etc.), offset by each map's position in the world.
--
-- Texture/animation loading is the one piece not done here: `_load_spritesheet` loads a
-- tileset's SpriteSheet resource through the engine's regular ResourceManager path (the same one
-- systems::Player uses for its Tileset), which owns the per-class Sprite animation data, read
-- back via `_SpriteSheet:get_sprite()`. Everything else about interpreting a tileset (atlas math,
-- gid resolution, collision shapes, custom properties) is done right here.

local function resolve_path(base_dir, relative_path)
    local parts = {}
    for part in base_dir:gmatch("[^/]+") do
        table.insert(parts, part)
    end
    for part in relative_path:gmatch("[^/]+") do
        if part == ".." then
            table.remove(parts)
        elseif part ~= "." then
            table.insert(parts, part)
        end
    end
    return table.concat(parts, "/")
end

local function dirname(path)
    return path:match("(.*)/[^/]+$") or "."
end

-- Mirrors engine::tilemap::Tileset::parse_shape(): a Tiled object is a point, a circular
-- ellipse (true ellipses are unsupported), or otherwise a rectangle.
local function shape_descriptor(object)
    if object.point then
        return {kind = "point", position = vec2(object.x, object.y)}
    elseif object.ellipse then
        if math.abs(object.width - object.height) > 1e-4 then
            error("Ellipses are not supported")
        end
        local radius = object.width / 2
        return {kind = "circle", position = vec2(object.x + radius, object.y + radius), radius = radius}
    else
        return {kind = "rectangle", position = vec2(object.x, object.y), dimensions = vec2(object.width, object.height)}
    end
end

-- A tile's collision shape is the first object of type "collision" in its objectgroup, if any.
local function tile_collision_shape(tile_def)
    if not tile_def or not tile_def.objectgroup or not tile_def.objectgroup.objects then
        return nil
    end
    for _, object in ipairs(tile_def.objectgroup.objects) do
        if object.type == "collision" then
            return shape_descriptor(object)
        end
    end
    return nil
end

local function read_property(properties, name)
    for _, property in ipairs(properties) do
        if property.name == name then
            return property.value
        end
    end
    return nil
end

-- Mirrors engine::graphics::SpriteSheet::source_rect(): the atlas cell for a tileset-local tile id.
local function calculate_source_rect(tileset, local_id)
    local column = local_id % tileset.columns
    local row = math.floor(local_id / tileset.columns)
    return _Rectangle(vec2(column * tileset.tile_dimensions.x, row * tileset.tile_dimensions.x), tileset.tile_dimensions)
end

local function load_tileset(tileset_ref, map_dir)
    local path = resolve_path(map_dir, tileset_ref.source)
    local dir = dirname(path)
    local data = _load_json(path)

    -- Loads the tileset's SpriteSheet resource through the engine (a no-op if this tileset was
    -- already loaded for an earlier map) - it owns the texture and per-class animation data.
    local spritesheet = _load_spritesheet(path)
    local tiles_by_id = {}
    for _, tile in ipairs(data.tiles or {}) do
        tiles_by_id[tile.id] = tile
    end

    local luminosity_path = data.properties.image_luminosity

    return {
        firstgid = tileset_ref.firstgid,
        tilecount = data.tilecount,
        columns = data.columns,
        tile_dimensions = spritesheet.tile_dimensions,
        texture = spritesheet.texture,
        luminosity = luminosity_path and _load_texture(resolve_path(dir, luminosity_path)) or nil,
        tiles_by_id = tiles_by_id,
        spritesheet = spritesheet,
    }
end

-- Mirrors engine::tilemap::TileMap::get(): finds which loaded tileset a global tile id falls
-- into (the one with the largest firstgid <= gid), and the tile's id local to that tileset.
local function resolve_gid(tilesets, gid)
    if not gid or gid == 0 then
        return nil
    end

    local tileset = nil
    for _, t in ipairs(tilesets) do
        if gid >= t.firstgid and (not tileset or t.firstgid > tileset.firstgid) then
            tileset = t
        end
    end
    if not tileset then
        return nil
    end

    local local_id = gid - tileset.firstgid
    if local_id < 0 or local_id >= tileset.tilecount then
        return nil
    end

    return tileset, local_id
end

local function create_tile_layer_entity(map_data, tilesets, layer, z_index, map_position)
    local cells = {}
    for y = 0, layer.height - 1 do
        for x = 0, layer.width - 1 do
            local index = x + layer.width * y
            local tileset, local_id = resolve_gid(tilesets, layer.data[index + 1])
            -- Always assigned (even for an empty cell, as `false`) so `cells` has no holes and
            -- its length reliably reflects the layer's tile count - see TileLayer::register_component().

            cells[index + 1] = tileset and {
                texture = tileset.texture,
                luminosity = tileset.luminosity,
                destination = _Rectangle(vec2(x * tileset.tile_dimensions.x, y * tileset.tile_dimensions.y), tileset.tile_dimensions),
                source = calculate_source_rect(tileset, local_id),
                shape = tile_collision_shape(tileset.tiles_by_id[local_id]),
            } or false
        end
    end

    local entity = _registry:create()
    _registry:emplace(entity, _Transform(map_position))
    _registry:emplace(entity, _Ordering(z_index))
    _registry:emplace(entity, _Renderable2D())
    _registry:emplace(entity, _Shape(_Point(vec2(layer.width, layer.height))))
    _registry:emplace(entity, _TileLayer(vec2(map_data.tilewidth, map_data.tileheight), cells))
end

local function create_sprite_entity(tilesets, object, map_position, map_dir)
    local tileset, local_id = resolve_gid(tilesets, object.gid)
    if not tileset then
        error("Tile not found")
    end

    local tile_def = tileset.tiles_by_id[local_id]
    if not tile_def then
        error("Tile not found")
    end

    local state_name = tile_def.properties.state
    local sprite_class = tileset.spritesheet:get_sprite(tile_def.type)
    local source_rect = calculate_source_rect(tileset, local_id)

    local position = vec2(map_position.x + object.x, map_position.y + object.y)

    local entity = _registry:create()
    _registry:emplace(entity, _Transform(position))
    _registry:emplace(entity, _Ordering(3.0))
    _registry:emplace(entity, _Renderable2D())

    --_registry:emplace(entity, _Animation(sprite_class, state_name))
    _registry:emplace(entity, _Sprite(tileset.texture, tileset.luminosity, source_rect))

    -- If the tile's state follows the "<direction>_<action>" convention and the class defines
    -- all four directions of that action, this sprite can be reoriented at runtime.
    
    --_make_orientable_if_directional(entity, sprite_class, state_name)
    local collision_shape = tile_collision_shape(tile_def)
    if collision_shape then
        _registry:emplace(entity, _Collider(collision_shape))
    end
    print("Hello")
    local script_path = read_property(object.properties, "script")
    if script_path then
        _run_script(resolve_path(map_dir, script_path), entity)
        _registry:emplace(entity, _Interactable())
    end
end

local function create_map(map_entry)
    local map_path = resolve_path("content/worlds", map_entry.fileName)
    local map_dir = dirname(map_path)
    local map_data = _load_json(map_path)
    local map_position = vec2(map_entry.x, map_entry.y)

    -- One entity per map, tagged _Map with a Transform (world position) and a Shape (pixel
    -- dimensions, as a Point) - together they give the map's world-space bounds rectangle - the
    -- ECS-native replacement for the former engine::tilemap::World resource (see
    -- systems::World::to_global()/map_at()).
    local map_dimensions = vec2(map_data.width * map_data.tilewidth, map_data.height * map_data.tileheight)
    local map_entity = _registry:create()
    _registry:emplace(map_entity, _Map())
    _registry:emplace(map_entity, _Transform(map_position))
    _registry:emplace(map_entity, _Shape(_Point(map_dimensions)))

    local tilesets = {}
    for _, tileset_ref in ipairs(map_data.tilesets) do
        table.insert(tilesets, load_tileset(tileset_ref, map_dir))
    end

    local z_index = 0
    for _, layer in ipairs(map_data.layers) do
        if layer.type == "tilelayer" then
            create_tile_layer_entity(map_data, tilesets, layer, z_index, map_position)
        elseif layer.type == "objectgroup" then
            for _, object in ipairs(layer.objects or {}) do
                if object.gid and object.gid > 0 then
                    create_sprite_entity(tilesets, object, map_position, map_dir)
                end
            end
        end
        z_index = z_index + 1
    end
end

local world = _load_json("content/worlds/world1.world")
for _, map_entry in ipairs(world.maps) do
    create_map(map_entry)
end
