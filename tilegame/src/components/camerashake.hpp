#pragma once

#include <string>

namespace tilegame::components
{
    // Per-axis screen shake state, owned by systems::Camera on its two dedicated shake entities
    // (one per axis - a Timer can't be shared between axes, see systems::Camera::load_content()).
    // While `settling` is false, systems::Camera picks a new random `target_offset` within
    // [-offset, +offset] every retarget_interval seconds and chases it, moving `current_offset`
    // towards it by at most displacement_speed units/second - a bounded random jitter whose
    // strength is `offset` and whose speed is `displacement_speed`, both given by the caller.
    // `settling` is set by this axis entity's own EventListener<TimerEvent> (see
    // systems::Camera::create_shake_axis_entity()) once its Timer rings, at which point
    // `target_offset` is pinned to 0 so the same chase logic doubles as the "smoothly settle
    // back to center" transition, until `current_offset` reaches (near) zero and the entity is
    // tagged Inactive again.
    struct CameraShakeAxis
    {
        float displacement_speed;
        float offset;
        float current_offset;
        float target_offset;
        float retarget_clock;
        bool settling;
    };

    // ---- Raised via System::raise() by the _shake_camera_horizontal Lua binding (see
    // Script::shake_camera_horizontal()), immediately delivered to systems::Camera's
    // EventListener<ShakeCameraHorizontalEvent> (registered on the horizontal shake axis entity
    // in Camera::load_content()). Restarts the horizontal shake axis, jittering within
    // [-offset, +offset] at up to `displacement_speed` units/second, for `duration` seconds.
    // Not exposed to Lua as a usertype - nothing outside systems::Camera subscribes to it today
    // - but carries EVENT_TYPE like any other event since System::raise_event() needs it
    // regardless.
    struct ShakeCameraHorizontalEvent
    {
        inline static const std::string EVENT_TYPE = "SHAKE_CAMERA_HORIZONTAL_EVENT";

        float displacement_speed;
        float offset;
        float duration;
    };

    // ---- Raised via System::raise() by the _shake_camera_vertical Lua binding (see
    // Script::shake_camera_vertical()), immediately delivered to systems::Camera's
    // EventListener<ShakeCameraVerticalEvent>. Restarts the vertical shake axis, jittering
    // within [-offset, +offset] at up to `displacement_speed` units/second, for `duration`
    // seconds.
    struct ShakeCameraVerticalEvent
    {
        inline static const std::string EVENT_TYPE = "SHAKE_CAMERA_VERTICAL_EVENT";

        float displacement_speed;
        float offset;
        float duration;
    };
} // namespace tilegame::components
