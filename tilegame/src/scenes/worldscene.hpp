#pragma once

#include <string>
#include <unordered_map>

#include "entt/entt.hpp"

#include "engine.hpp"

#include "systems/render.hpp"
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
#include "systems/weather.hpp"
#include "systems/lightning.hpp"
#include "systems/collisiondetection.hpp"
#include "systems/messagebox.hpp"
#include "systems/interaction.hpp"
#include "systems/facing.hpp"
#include "systems/spriteorientation.hpp"

namespace tilegame::scenes
{
    /**
     * @brief Top-level orchestrator for the currently loaded game scene.
     *
     * Owns the entt::registry and one instance of every systems::* system, and
     * drives them through their lifecycle each frame in the fixed order
     * required for correct behavior (e.g. input/scripts before movement,
     * collision before movement is applied, animation/camera after). Pushed
     * as the startup scene by Tilegame; can itself push overlay scenes (e.g.
     * an inventory) on top via the SceneManager reached through game().scene_manager().
     */
    class WorldScene : public engine::Scene
    {
    private:
        entt::registry _registry;

        systems::Render _system_render;
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
        systems::Weather _system_weather;
        systems::Lightning _system_lightning;
        systems::CollisionDetection _system_collision_detection;
        systems::MessageBox _system_messagebox;
        systems::Interaction _system_interaction;
        systems::Facing _system_facing;
        systems::SpriteOrientation _system_sprite_orientation;

    public:
        WorldScene(engine::Game &game);
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