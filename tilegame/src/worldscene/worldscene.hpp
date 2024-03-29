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
#include "systems/movementcontroller.hpp"
#include "systems/pin.hpp"
#include "systems/animation.hpp"
#include "systems/script.hpp"
#include "systems/timer.hpp"
#include "systems/particle.hpp"
#include "systems/daytime.hpp"
#include "systems/collisiondetection.hpp"

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
        engine::graphics::PostProcessor _postprocessor;

        systems::Render _system_render;
        systems::Map _system_map;
        systems::Camera _system_camera;
        systems::Player _system_player;
        systems::Movement _system_movement;
        systems::MovementController _system_movement_controller;
        systems::Pin _system_pin;
        systems::Animation _system_animation;
        systems::Script _system_script;
        systems::Timer _system_timer;
        systems::Particle _system_particle;
        systems::Daytime _system_daytime;
        systems::CollisionDetection _system_collision_detection;

    public:
        WorldScene(Tilegame &game);
        ~WorldScene() = default;

        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(const engine::GameTime &update_time) override;
        virtual void end_update() override;
        virtual void begin_draw() override;
        virtual void draw(const engine::GameTime &draw_time) override;
        virtual void end_draw(const engine::GameTime &draw_time) override;
    };
} // namespace tilegame::worldscene