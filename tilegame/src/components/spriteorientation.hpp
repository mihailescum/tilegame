#pragma once

#include <array>
#include <string>

#include <glm/glm.hpp>

#include "entt/entt.hpp"
#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Attached, alongside Facing, to sprite entities whose class defines all four
     * directional variants of one action (see Direction) - by systems::Player directly (via
     * make_orientable_if_directional()) for the keyboard-controlled player, and by Lua-native
     * code (content/scripts/maploader.lua's `make_orientable_if_directional`, built from the
     * `_SpriteOrientation`/`_Facing` constructors below) for map-loaded sprite entities - so
     * systems::SpriteOrientation can swap their Animation/Sprite to match Facing whenever it
     * crosses into a new cardinal direction.
     * `sprite` is the per-class engine::graphics::Sprite (map of state name -> SpriteState),
     * resolved once at spawn and stable for the rest of the session (it points into the owning
     * tileset's SpriteSheet, a ResourceManager-owned resource kept alive for the session like any
     * other); `action` is the fixed `<action>` suffix (e.g. "walking") this
     * entity was spawned with - this only ever
     * swaps direction, never action.
     */
    struct SpriteOrientation
    {
        /// The four cardinal directions a sprite class can define in the `<direction>_<action>`
        /// SpriteState lookup naming convention (e.g. "down_walking"). Everything outside
        /// direction_prefix() (which only exists to build that string convention) deals in this
        /// enum, never the raw prefix strings.
        enum class Direction
        {
            Up,
            Down,
            Left,
            Right
        };

        static constexpr std::array<Direction, 4> DIRECTIONS = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};

        /// The `<direction>_` prefix a Direction corresponds to (e.g. Direction::Down -> "down"),
        /// needed only to build the (string-keyed) SpriteState lookup name.
        static std::string direction_prefix(Direction direction);

        /// Maps a (non-zero) heading to the closest Direction, by whichever axis dominates.
        static Direction direction_from_heading(const glm::vec2 &heading);

        /// Inverse of direction_from_heading(), for initializing Facing to the one direction a
        /// SpriteOrientation entity is spawned already showing.
        static glm::vec2 direction_vector(Direction direction);

        /// If `sprite` defines `action` in all four Directions, emplaces Facing (initialized to
        /// `initial_direction`) and SpriteOrientation on `entity`, so systems::SpriteOrientation
        /// can reorient it at runtime as its Facing changes. Otherwise a no-op - e.g. for a
        /// sprite class that doesn't define all four directions of `action`.
        /// Used by systems::Player (for the keyboard-controlled player, which follows the same
        /// "man" class/state convention). Map-loaded sprite entities get the same treatment from
        /// Lua-native code instead (see content/scripts/maploader.lua's
        /// `make_orientable_if_directional`, built from the `_Facing`/`_SpriteOrientation`
        /// constructors and `_SpriteClass:has_state()`) - this duplicates the has-all-directions
        /// check here in Lua, but only until player loading moves to Lua too.
        static void make_orientable_if_directional(entt::registry &registry, entt::entity entity, const engine::graphics::Sprite &sprite, Direction initial_direction, const std::string &action);

        /// Registers `_SpriteOrientation`, constructible from Lua as
        /// `_SpriteOrientation(sprite_class, action, heading)` - `heading` (a non-zero vec2) is
        /// mapped to `current_direction` via direction_from_heading(), the same as `_Facing`'s own
        /// initial direction, so Lua code emplaces both from the one heading it already computed.
        static void register_component(sol::state &lua);

        const engine::graphics::Sprite *sprite;
        std::string action;
        /// Direction last applied, so systems::SpriteOrientation can skip redundant
        /// lookups/patches once Facing hasn't crossed into a new cardinal direction.
        Direction current_direction;
    };
} // namespace tilegame::components
