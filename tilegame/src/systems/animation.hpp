#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Advances frame-based sprite animations over time.
     *
     * Operates on entities with an Animation component, advancing the current
     * frame according to its duration once enough time has elapsed; entities
     * that also carry a Sprite component get their source rect updated to the
     * new frame each time it changes.
     */
    class Animation : public System
    {
    public:
        Animation(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame