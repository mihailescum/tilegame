#include "mapsystem.hpp"

#include "components/renderable2d.hpp"
#include "components/worldtransform.hpp"

namespace tilegame::systems
{
    MapSystem::MapSystem(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    const entt::entity MapSystem::create_map_entity_from_file(const std::string name, const std::string path)
    {
        auto &resource_manager = _scene.get_game().get_resource_manager();
        auto map_ptr = resource_manager.load_resource<engine::tilemap::TileMap>(name, path);
        if (map_ptr)
        {
            auto map = *map_ptr;

            const auto entity = _registry.create();

            _registry.emplace<tilegame::components::WorldTransform>(entity, glm::vec2(-20.0, -50.0));
            _registry.emplace<tilegame::components::Renderable2D>(entity, map.get_tilesets()[0].get_texture());

            return entity;
        }
        else
        {
            throw "Could not load map";
        }
    }
}