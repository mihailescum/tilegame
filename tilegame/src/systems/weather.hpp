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
     * precipitation particle emitter entity is also created once, in load_content() - "no
     * precipitation" is represented by that entity carrying an Inactive tag, not by it being
     * absent. It is Pin'd to the camera entity (itself Pin'd to player 1, see
     * systems::Camera::load_content()) - so load_content() must run after
     * systems::Camera::load_content() - meaning its spawn area travels with the camera's view
     * instead of being recomputed every frame; the spawn area itself (a Shape/Rectangle sized and
     * positioned relative to that pin, e.g. a band above the screen so rain enters from the top
     * rather than popping in everywhere at once) is entirely Lua's call, passed in on each
     * SetWeatherPrecipitationEvent. All three event types (SetWeatherTintEvent too) are handled
     * by EventListener<T>s registered on a *separate* dedicated entity, never tagged Inactive -
     * since the precipitation entity starts Inactive and raise_event()'s listener view excludes
     * Inactive entities, listeners meant to remove that tag can't live on the entity they'd be
     * removing it from. All three listeners fire the instant Script raises the matching event via
     * the inherited System::raise(). Every frame, this system only advances the tint fade
     * (uploading it to the daytime post-processing shader's `weather_tint` uniform, see
     * content/shaders/daytime.frag). It has no knowledge of systems::Script/Lua at all.
     */
    class Weather : public System
    {
    private:
        engine::Shader *_daytime_shader;

        engine::Color _start_tint;
        engine::Color _target_tint;
        float _fade_elapsed;
        float _fade_duration;

        // Creates the precipitation entity, called once from load_content(): Transform/Shape, a
        // default ParticleEmitter (replaced wholesale by the first
        // SetWeatherPrecipitationEvent), a fresh ParticlePool, and the Renderable2D/Ordering
        // pair needed for systems::Particle/systems::Render to pick it up. Pinned to the camera
        // entity.
        entt::entity create_precipitation_entity();

    public:
        Weather(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
