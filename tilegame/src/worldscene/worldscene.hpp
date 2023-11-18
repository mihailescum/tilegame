#pragma once

#include <string>
#include <unordered_map>

#include <entt/entity/registry.hpp>

#include "engine.hpp"

#include "scenegraphdata.hpp"
#include "systems/render.hpp"
#include "systems/map.hpp"
#include "systems/camera.hpp"
#include "systems/player.hpp"
#include "systems/movement.hpp"
#include "systems/scenegraph.hpp"
#include "systems/animation.hpp"
#include "systems/script.hpp"
#include "systems/timer.hpp"

namespace tilegame
{
    class Tilegame;
}

namespace tilegame::worldscene
{
    class WorldScene : public tilegame::Scene
    {
    private:
        entt::registry _registry;

        tilegame::systems::RenderSystem _system_render;
        tilegame::systems::MapSystem _system_map;
        tilegame::systems::CameraSystem _system_camera;
        tilegame::systems::PlayerSystem _system_player;
        tilegame::systems::MovementSystem _system_movement;
        tilegame::systems::SceneGraphSystem _system_scenegraph;
        tilegame::systems::AnimationSystem _system_animation;
        tilegame::systems::ScriptSystem _system_script;
        tilegame::systems::TimerSystem _system_timer;

    public:
        WorldScene(Tilegame &game);
        ~WorldScene() = default;

        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(const engine::GameTime &update_time) override;
        virtual void draw(const engine::GameTime &draw_time) override;
    };
} // namespace tilegame::worldscene