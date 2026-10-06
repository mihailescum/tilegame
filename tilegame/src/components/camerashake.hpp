#pragma once

#include <optional>
#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    // Parameters for shaking one camera axis: jitter within [-offset, +offset] world units at
    // up to `displacement_speed` units/second, for `duration` seconds, after which
    // systems::Camera smoothly settles it back to center at the same speed.
    struct CameraShakeAxisSettings
    {
        float displacement_speed;
        float offset;
        float duration;
    };

    // Per-axis state of one running shake (see CameraShake). While `settling` is false,
    // systems::Camera counts `remaining_duration` down, picking a new random `target_offset`
    // within [-offset, +offset] at a fixed interval and chasing it, moving `current_offset`
    // towards it by at most displacement_speed units/second - a bounded random jitter. Once
    // `remaining_duration` runs out (or the shake is stopped early, see StopCameraShakeEvent),
    // `settling` is set and `target_offset` pinned to 0, so the same chase logic doubles as the
    // "smoothly settle back to center" transition, until `current_offset` reaches (near) zero
    // and the axis is done.
    struct CameraShakeAxis
    {
        float displacement_speed;
        float offset;
        float remaining_duration;
        float current_offset = 0.0f;
        float target_offset = 0.0f;
        float retarget_clock = 0.0f;
        bool settling = false;

        CameraShakeAxis(const CameraShakeAxisSettings &settings)
            : displacement_speed(settings.displacement_speed), offset(settings.offset), remaining_duration(settings.duration) {}
    };

    // One running screen shake, on its own entity (created by Script::shake_camera(), which
    // returns it to Lua as the shake's handle). Any number can run at once; systems::Camera
    // sums every shake's per-axis offsets. An axis is empty if the shake never shook it or it
    // has already finished settling; once both are, systems::Camera raises
    // CameraShakeEndedEvent (source = this entity) and destroys the entity.
    struct CameraShake
    {
        std::optional<CameraShakeAxis> horizontal;
        std::optional<CameraShakeAxis> vertical;
    };

    // ---- Raised via System::raise() by the _shake_camera Lua binding (see
    // Script::shake_camera()) with the freshly created shake entity as target, immediately
    // delivered to systems::Camera's EventListener<ShakeCameraEvent> (registered on its control
    // entity in Camera::load_content()), which emplaces a CameraShake on that target for
    // whichever axes are given. Not exposed to Lua as a usertype - nothing outside
    // systems::Camera subscribes to it today - but carries EVENT_TYPE like any other event since
    // System::raise_event() needs it regardless.
    struct ShakeCameraEvent
    {
        inline static const std::string EVENT_TYPE = "SHAKE_CAMERA_EVENT";

        std::optional<CameraShakeAxisSettings> horizontal;
        std::optional<CameraShakeAxisSettings> vertical;
    };

    // ---- Raised via System::raise() by the _stop_camera_shake Lua binding (see
    // Script::stop_camera_shake()) with the shake entity as target, immediately delivered to
    // systems::Camera, which ends that shake's jitter early: every remaining axis starts
    // settling back to center right away, after which CameraShakeEndedEvent is raised as usual.
    // A no-op if the target isn't a running shake (e.g. it already ended).
    struct StopCameraShakeEvent
    {
        inline static const std::string EVENT_TYPE = "STOP_CAMERA_SHAKE_EVENT";
    };

    /**
     * @brief Raised by systems::Camera once per shake, with the shake's entity (as returned by
     * `_shake_camera`) as source, on the frame its last axis finishes settling back to center -
     * so a shake with a 2s horizontal and 5s vertical axis raises it once, after the vertical
     * one has settled. Other shakes still running don't delay it. The entity is destroyed right
     * after. Exposed to Lua as `_CameraShakeEndedEvent`; filter on the source to wait for one
     * specific shake.
     */
    struct CameraShakeEndedEvent
    {
        inline static const std::string EVENT_TYPE = "CAMERA_SHAKE_ENDED_EVENT";

        CameraShakeEndedEvent() = default;
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
