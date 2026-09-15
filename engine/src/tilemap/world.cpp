#include "tilemap/world.hpp"

#include "tileson/tileson.hpp"

#include "core/log.hpp"
#include "core/resourcemanager.hpp"

namespace engine::tilemap
{
    bool World::load_resource(ResourceManager &resource_manager, va_list args)
    {
        tson::World tson_world;
        if (!tson_world.parse(_resource_path))
        {
            Log::e("Failed to load world. FILE: ", _resource_path);
            return false;
        }

        for (const auto &map_data : tson_world.getMapData())
        {
            const std::string map_name = map_data.path.stem().string();
            const glm::vec2 position(map_data.position.x, map_data.position.y);
            const glm::vec2 size(map_data.size.x, map_data.size.y);

            const TileMap *map = resource_manager.load_resource<TileMap>(map_name, map_data.path);
            if (map)
            {
                _maps.emplace(map_name, MapEntry{map, position, size});
            }
            else
            {
                Log::e("Failed to load map referenced by world. FILE: ", map_data.path);
            }
        }

        return true;
    }

    void World::unload_resource()
    {
    }

    glm::vec2 World::to_global(const std::string &map_name, const glm::vec2 &relative_position) const
    {
        const auto it = _maps.find(map_name);
        if (it == _maps.end())
        {
            throw "Map not found";
        }

        return it->second.position + relative_position;
    }

    std::string World::map_at(const glm::vec2 &global_position) const
    {
        for (const auto &[map_name, entry] : _maps)
        {
            const bool inside_x = global_position.x >= entry.position.x && global_position.x < entry.position.x + entry.size.x;
            const bool inside_y = global_position.y >= entry.position.y && global_position.y < entry.position.y + entry.size.y;
            if (inside_x && inside_y)
            {
                return map_name;
            }
        }

        return "";
    }
} // namespace engine::tilemap
