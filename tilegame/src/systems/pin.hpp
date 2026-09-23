#pragma once

#include <string>

#include "engine.hpp"

namespace tilegame::systems
{
    /**
     * @brief Keeps an entity's position locked to another entity's position.
     *
     * Operates on entities with a Transform + Pin component, copying the
     * pinned target entity's Transform position onto them every frame (e.g.
     * used to make the camera follow the player).
     */
    class Pin : public engine::System
    {
    public:
        Pin(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame