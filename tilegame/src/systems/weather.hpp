#pragma once

#include "engine.hpp"
#include "entt/entt.hpp"

#include "system.hpp"
#include "components/weather.hpp"

namespace tilegame::systems
{
    /**
     * @brief Drives the weather overlay tint and precipitation particles, composed with the
     * day/night tint.
     *
     * Owns the tint fade state privately (current/start/target tint, fade progress). The
     * precipitation particle emitter entity is created once, in load_content(), and its handle
     * is stored for the rest of the program's life in the registry context (registry.ctx())
     * under a private hashed id (see weather.cpp) rather than as a system member - "no
     * precipitation" is represented by that entity carrying an Inactive tag, not by it being
     * absent. It is Pin'd to player 1 (the same way systems::Camera pins itself), so its spawn
     * area travels with the player instead of being recomputed from the camera every frame; the
     * spawn area itself (a Shape/Rectangle sized and positioned relative to that pin, e.g. a
     * band above the screen so rain enters from the top rather than popping in everywhere at
     * once) is entirely Lua's call, passed in on each SetWeatherPrecipitationEvent. Every frame,
     * this system only applies pending commands - raised by Lua via Script, on their own
     * throwaway entities - and advances the tint fade (uploading it to the daytime
     * post-processing shader's `weather_tint` uniform, see content/shaders/daytime.frag). It has
     * no knowledge of systems::Script/Lua or systems::Camera at all.
     */
    class Weather : public System
    {
    private:
        engine::Shader *_daytime_shader;

        engine::Color _start_tint;
        engine::Color _target_tint;
        float _fade_elapsed;
        float _fade_duration;

        void apply_pending_commands();
        // Creates the precipitation entity, called once from load_content(): Transform/Shape, a
        // default ParticleEmitter (replaced wholesale by the first
        // SetWeatherPrecipitationEvent), a fresh ParticlePool, and the Renderable2D/Ordering
        // pair needed for systems::Particle/systems::Render to pick it up. Pinned to player 1,
        // if found.
        entt::entity create_precipitation_entity();

    public:
        Weather(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
