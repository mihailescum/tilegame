#pragma once

#include <string>
#include <unordered_map>

#include <entt/entity/registry.hpp>

#include "engine.hpp"

#include "scenegraphdata.hpp"
#include "systems/rendersystem.hpp"
#include "systems/mapsystem.hpp"
#include "systems/camerasystem.hpp"
#include "systems/playersystem.hpp"
#include "systems/movementsystem.hpp"

namespace tilegame
{
    class Tilegame;
}

namespace tilegame::worldscene
{
    class WorldScene : public engine::Scene
    {
    private:
        entt::registry _registry;

        tilegame::SceneGraphNode _scene_graph_root;

        tilegame::systems::RenderSystem _system_render;
        tilegame::systems::MapSystem _system_map;
        tilegame::systems::CameraSystem _system_camera;
        tilegame::systems::PlayerSystem _system_player;
        tilegame::systems::MovementSystem _system_movement;

    public:
        WorldScene(Tilegame &game);
        ~WorldScene() = default;

        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(const engine::GameTime &update_time) override;
        virtual void draw(const engine::GameTime &draw_time) override;

        tilegame::SceneGraphNode &get_scene_graph_root() { return _scene_graph_root; }
    };
} // namespace tilegame::worldscene