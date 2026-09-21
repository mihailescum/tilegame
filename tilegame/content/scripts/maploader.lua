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

-- Maps a tile layer's index in its Tiled file into an actual Z (components::Depth.z is used
-- as-is by systems::Render, with no further scaling - see that component). SpriteBatch's ortho
-- projection covers Z in [-1, 1] with GL_GREATER (a larger Z wins - see
-- engine::graphics::SpriteBatch::begin()), so a higher layer index, meant to sit in front of
-- lower ones, gets a larger value. All tiles are static for now - every cell in a layer shares
-- this one Z; there is no per-tile/dynamic case.
local function layer_z(z_index)
    return z_index * 0.01
end

local function read_property(properties, name)
    -- A Tiled tile/object with no custom properties at all omits the "properties" key entirely
    -- (e.g. a tile with only an objectgroup for collision) rather than giving an empty array.
    if not properties then
        return nil
    end
    for _, property in ipairs(properties) do
        if property.name == name then
            return property.value
        end
    end
    return nil
end

-- The shared bucket characters sort in - see player.cpp's and create_sprite_entity's own
-- _Depth(0.5); kept in sync with those by hand, since there's no single shared constant for it
-- on the Lua side.
local DYNAMIC_DEPTH = 0.5

-- A tile's depth anchor is its custom property `depth_anchor`, if present - marks it as needing
-- to sort against characters using its own row (see systems::Render's draw_tilelayer) instead of
-- sitting in its layer's static bucket, e.g. a tree that should occlude/be occluded by the player
-- depending on position. 0 anchors to the tile's own bottom pixel row; negative values push the
-- reference row further down past that edge, positive values pull it up (useful for e.g.
-- transparent padding under the actual ground-contact pixel). A tall object spanning several
-- grid cells (canopy above trunk) marks each tile definition independently, each with its own
-- anchor - no linking needed, since each cell already sorts by its own row regardless.
local function tile_depth_anchor(tile_def)
    if not tile_def then
        return nil
    end
    return read_property(tile_def.properties, "depth_anchor")
end

-- Mirrors components::SpriteOrientation::direction_vector(): the heading each cardinal
-- direction's Facing/SpriteOrientation is initialized to.
local DIRECTION_HEADINGS = {
    up = vec2(0, -1),
    down = vec2(0, 1),
    left = vec2(-1, 0),
    right = vec2(1, 0),
}

-- If `sprite_class` defines `action` in all four directions, emplaces Facing (initialized to
-- `initial_direction`) and SpriteOrientation on `entity`, so systems::SpriteOrientation can
-- reorient it at runtime as its Facing changes. Otherwise a no-op - e.g. for a sprite class that
-- doesn't define all four directions of `action`. Lua-native counterpart of
-- components::SpriteOrientation::make_orientable_if_directional(), which systems::Player still
-- calls directly in C++ for the keyboard-controlled player - duplicated here only until player
-- loading moves to Lua too.
local function make_orientable_if_directional(entity, sprite_class, initial_direction, action)
    for direction in pairs(DIRECTION_HEADINGS) do
        if not sprite_class:has_state(direction .. "_" .. action) then
            return
        end
    end

    _registry:emplace(entity, _Facing(DIRECTION_HEADINGS[initial_direction]))
    _registry:emplace(entity, _SpriteOrientation(sprite_class, action, DIRECTION_HEADINGS[initial_direction]))
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
    -- Baked into each cell at load time (see TileLayer::TileData::depth) - this layer's own
    -- static bucket, unless that specific tile has a depth_anchor (see tile_depth_anchor), in
    -- which case it joins DYNAMIC_DEPTH's bucket instead.
    local layer_depth = layer_z(z_index)

    local cells = {}
    for y = 0, layer.height - 1 do
        for x = 0, layer.width - 1 do
            local index = x + layer.width * y
            local tileset, local_id = resolve_gid(tilesets, layer.data[index + 1])
            -- Always assigned (even for an empty cell, as `false`) so `cells` has no holes and
            -- its length reliably reflects the layer's tile count - see TileLayer::register_component().

            local tile_def = tileset and tileset.tiles_by_id[local_id]
            local anchor = tile_depth_anchor(tile_def)
            local depth = anchor and DYNAMIC_DEPTH or layer_depth
            -- The tile's own bottom pixel row when there's no anchor (anchor defaults to 0) -
            -- see systems::Render, which reads this directly rather than deriving it from the
            -- cell's own destination rect. nil (unused) for an empty cell, same as everything
            -- else computed from `tileset` below.
            local reference_y = tileset and (map_position.y + (y + 1) * tileset.tile_dimensions.y - (anchor or 0))

            cells[index + 1] = tileset and {
                texture = tileset.texture,
                luminosity = tileset.luminosity,
                destination = _Rectangle(vec2(x * tileset.tile_dimensions.x, y * tileset.tile_dimensions.y), tileset.tile_dimensions),
                source = calculate_source_rect(tileset, local_id),
                depth = depth,
                reference_y = reference_y,
                shape = tile_collision_shape(tile_def),
            } or false
        end
    end

    -- No _Depth on the layer entity itself - only individual cells carry a baked depth (above),
    -- which is all systems::Render reads for tile layers now.
    local entity = _registry:create()
    _registry:emplace(entity, _Transform(map_position))
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

    -- Mirrors engine::graphics::Sprite::parse(): the SpriteState lookup key is "<direction>_
    -- <action>" when the tile has a "direction" property (see characters.tsj), otherwise just
    -- "<action>" as-is.
    local action = read_property(tile_def.properties, "state")
    local direction = read_property(tile_def.properties, "direction")
    local state_name = direction and (direction .. "_" .. action) or action
    local sprite_class = tileset.spritesheet:get_sprite(tile_def.type)
    local source_rect = calculate_source_rect(tileset, local_id)

    local position = vec2(map_position.x + object.x, map_position.y + object.y)

    local entity = _registry:create()
    _registry:emplace(entity, _Transform(position))
    -- Arbitrary placeholder for now, same as the player (see player.cpp) - characters aren't
    -- placed into the layer_z() binning above yet, just kept safely above every tile layer's Z
    -- (0 and ascending) so they win the (GL_GREATER - larger wins) depth test against the ground.
    _registry:emplace(entity, _Depth(0.5))
    _registry:emplace(entity, _Renderable2D())

    _registry:emplace(entity, _Animation(sprite_class, state_name))
    _registry:emplace(entity, _Sprite(tileset.texture, tileset.luminosity, source_rect))

    -- If the tile has a direction and the class defines all four directions of its action, this
    -- sprite can be reoriented at runtime.
    if direction then
        make_orientable_if_directional(entity, sprite_class, direction, action)
    end
    local collision_shape = tile_collision_shape(tile_def)
    if collision_shape then
        _registry:emplace(entity, _Collider(collision_shape))
    end
    
    local script_path = read_property(object.properties, "script")
    if script_path then
        _run_script(resolve_path(map_dir, script_path), entity)
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

    local script_path = read_property(map_data.properties, "script")
    if script_path then
        _run_script(resolve_path(map_dir, script_path), map_entity)
    end

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
