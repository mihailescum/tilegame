#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Keeps a directional sprite's Animation/Sprite in sync with its Facing.
     *
     * Each frame, for every entity carrying Facing, Animation and components::SpriteOrientation
     * (emplaced by systems::Script's `_make_orientable_if_directional` binding or systems::Player
     * - see components::SpriteOrientation::make_orientable_if_directional() - on sprite entities whose
     * class defines all four directional variants of one action), maps Facing to the closest
     * cardinal direction (components::SpriteOrientation::direction_from_heading) and, if that
     * differs from the direction currently applied, swaps Animation's frames to the matching
     * SpriteState and resets its playback - also patching Sprite's source_rect immediately, since
     * systems::Animation only refreshes it when a frame naturally times out. Must run after
     * systems::Facing.
     */
    class SpriteOrientation : public System
    {
    public:
        SpriteOrientation(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
