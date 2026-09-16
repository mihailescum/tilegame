#pragma once

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

    // ---- Command: emplaced on a fresh entity by the _shake_camera_horizontal Lua binding (see
    // Script::shake_camera_horizontal()), consumed and destroyed by systems::Camera the next
    // time it updates. Restarts the horizontal shake axis, jittering within [-offset, +offset]
    // at up to `displacement_speed` units/second, for `duration` seconds.
    struct ShakeCameraHorizontalEvent
    {
        float displacement_speed;
        float offset;
        float duration;
    };

    // ---- Command: emplaced on a fresh entity by the _shake_camera_vertical Lua binding (see
    // Script::shake_camera_vertical()), consumed and destroyed by systems::Camera the next time
    // it updates. Restarts the vertical shake axis, jittering within [-offset, +offset] at up
    // to `displacement_speed` units/second, for `duration` seconds.
    struct ShakeCameraVerticalEvent
    {
        float displacement_speed;
        float offset;
        float duration;
    };
} // namespace tilegame::components
