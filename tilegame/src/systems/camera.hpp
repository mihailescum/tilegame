#pragma once

#include <string>

#include "engine.hpp"
#include "entt/entt.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Maintains the game's single camera view transform, including screen shake.
     *
     * Creates the camera entity (Camera + Transform components) on
     * load_content, pinning it to player 1 via a Pin component so it follows
     * the player and storing its handle in the registry context under
     * components::CAMERA_ENTITY_ID (there is only ever one camera, so a
     * named ctx() entry stands in for what would otherwise be a
     * single-entity view), and each frame recomputes the camera's
     * translate/scale transform matrix (and derived world-space
     * visible_bounds) from its Transform position, viewport, and any active
     * screen shake.
     *
     * Screen shake has its own two dedicated entities (horizontal/vertical - a Timer can't be
     * shared between axes), also tracked via private registry context ids rather than system
     * members. Each carries a components::Timer (decremented by the shared systems::Timer, so
     * this system never rolls its own elapsed-time bookkeeping), a components::CameraShakeAxis
     * holding the jitter state, and its own components::EventListener<TimerEvent> (source-filtered
     * to itself) that flips CameraShakeAxis::settling once its Timer rings - the same
     * native-event mechanism Lua uses via _add_event_listener, just consumed directly in C++
     * instead of forwarding into a Lua callback. See camera.cpp for the per-axis update. Lua
     * triggers/restarts a shake via components::ShakeCameraHorizontalEvent/ShakeCameraVerticalEvent
     * (raised by Script::shake_camera_horizontal()/shake_camera_vertical(), on their own
     * throwaway entities), consumed the same way systems::Weather consumes its Set*Event commands.
     */
    class Camera : public System
    {
    private:
        void apply_pending_commands();
        entt::entity create_shake_axis_entity();
        // Advances one shake axis by `elapsed_time` and returns its current offset (0 if
        // Inactive, i.e. not currently shaking or settling). See components::CameraShakeAxis.
        float update_shake_axis(entt::entity axis_entity, float elapsed_time);

    public:
        Camera(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
