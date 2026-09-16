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
     * shared between axes), tracked via private registry context ids rather than system
     * members, each starting Inactive (not currently shaking) and carrying a components::Timer
     * (decremented by the shared systems::Timer), a components::CameraShakeAxis holding the
     * jitter state, and its own components::EventListener<TimerEvent> (source-filtered to
     * itself, registered in create_shake_axis_entity()) that flips CameraShakeAxis::settling
     * once its Timer rings. A third, separate control entity (created in load_content(), never
     * tagged Inactive - see the comment there for why) carries
     * EventListener<ShakeCameraHorizontalEvent>/EventListener<ShakeCameraVerticalEvent>, which
     * (re)start the matching axis the instant Script::shake_camera_horizontal()/
     * shake_camera_vertical() raises it via the inherited System::raise(). See camera.cpp for
     * the per-axis update.
     */
    class Camera : public System
    {
    private:
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
