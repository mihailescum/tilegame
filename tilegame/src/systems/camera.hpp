#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Maintains the game camera's view transform.
     *
     * Creates the camera entity (Camera + Transform components) on
     * load_content, pinning it to player 1 via a Pin component so it follows
     * the player, and each frame recomputes the camera's translate/scale
     * transform matrix (and derived world-space visible_bounds) from its
     * Transform position and viewport.
     */
    class Camera : public System
    {
    public:
        Camera(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame