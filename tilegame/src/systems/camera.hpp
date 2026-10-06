#pragma once

#include <optional>
#include <string>

#include "engine.hpp"
#include "entt/entt.hpp"

#include "components/camerashake.hpp"

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
     * Screen shake: every running shake is its own entity carrying a components::CameraShake
     * (see there). A separate control entity (created in load_content()) carries the
     * EventListener<ShakeCameraEvent>/EventListener<StopCameraShakeEvent> that start/stop a
     * shake the instant Script::shake_camera()/stop_camera_shake() raises one via the
     * inherited System::raise(). Each frame, update() advances every shake, adds their offsets
     * together, and raises components::CameraShakeEndedEvent (source = the shake entity) for,
     * then destroys, each shake whose axes have all settled.
     */
    class Camera : public engine::System
    {
    private:
        // Advances one shake axis by `elapsed_time` and returns its current offset. Resets
        // `axis` to empty once it has finished settling. See components::CameraShakeAxis.
        float update_shake_axis(std::optional<components::CameraShakeAxis> &axis, float elapsed_time);
        // Advances every running shake and returns their summed offset, raising
        // CameraShakeEndedEvent for and destroying any shake that finished this frame.
        glm::vec2 update_shakes(float elapsed_time);

    public:
        Camera(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
