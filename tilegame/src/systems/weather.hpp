#pragma once

#include "engine.hpp"
#include "entt/entt.hpp"

#include "components/weather.hpp"

namespace tilegame::systems
{
    /**
     * @brief Drives the weather overlay tint and precipitation particles, composed with the
     * day/night tint.
     *
     * Owns the tint fade state privately (current/start/target tint, fade progress).
     *
     * Precipitation: every SetWeatherPrecipitationEvent creates a new precipitation entity (see
     * create_precipitation_entity()), tagged components::Precipitation, and stops the one that
     * was emitting before by removing its ParticleEmitter - systems::Particle only spawns for
     * entities that have one, but keeps ageing (and systems::Render keeps drawing) a bare
     * ParticlePool, so the old effect's particles finish falling while the new one starts. A
     * ClearWeatherPrecipitationEvent stops the emitting one the same way without starting a
     * new one. Each frame, update() destroys any stopped precipitation entity whose pool has no
     * live particles left, along with its pooled particle entities. The spawn area itself (a
     * Shape/Rectangle relative to the camera the entity is pinned to, e.g. a band above the
     * screen so rain enters from the top rather than popping in everywhere at once) is
     * entirely Lua's call, passed in on each SetWeatherPrecipitationEvent.
     *
     * All three event types (SetWeatherTintEvent too) are handled by EventListener<T>s
     * registered on a dedicated, never Inactive entity in load_content(), and fire the instant
     * Script raises the matching event via the inherited System::raise(). Every frame, update()
     * also advances the tint fade (uploading it to the daytime post-processing shader's
     * `weather_tint` uniform, see content/shaders/daytime.frag). It has no knowledge of
     * systems::Script/Lua at all.
     */
    class Weather : public engine::System
    {
    private:
        engine::Shader *_daytime_shader;

        engine::Color _start_tint;
        engine::Color _target_tint;
        float _fade_elapsed;
        float _fade_duration;

        // Creates a new precipitation entity for one SetWeatherPrecipitationEvent: the
        // Precipitation tag, Transform, `spawn_area` as its Shape, `emitter`, a fresh
        // ParticlePool, and the Renderable2D/Depth pair needed for systems::Particle/
        // systems::Render to pick it up. Pinned to the camera entity.
        entt::entity create_precipitation_entity(const components::ParticleEmitter &emitter, const engine::Rectangle &spawn_area);
        // Removes the ParticleEmitter from every precipitation entity still emitting, so they
        // stop spawning but let their live particles finish.
        void stop_precipitation();
        // Destroys every stopped precipitation entity whose pool has no live particles left,
        // together with its pooled (all Inactive by then) particle entities.
        void destroy_finished_precipitation();

    public:
        Weather(engine::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
