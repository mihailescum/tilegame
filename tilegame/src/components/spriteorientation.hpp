#pragma once

#include <array>
#include <optional>
#include <string>
#include <utility>

#include <glm/glm.hpp>

#include "entt/entt.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Attached, alongside Facing, by make_orientable_if_directional() (called from both
     * systems::Script - via its `_make_orientable_if_directional` binding, for map-loaded sprite
     * entities - and systems::Player) to sprite entities whose class defines all four
     * directional variants of one action (see Direction), so systems::SpriteOrientation can swap
     * their Animation/Sprite to match Facing whenever it crosses into a new cardinal direction.
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
        /// state naming convention (e.g. "down_walking"). Everything outside direction_prefix()
        /// and split_state_name() (which only exist to cross to/from that string convention) and
        /// the final SpriteState lookup deals in this enum, never the raw prefix strings.
        enum class Direction
        {
            Up,
            Down,
            Left,
            Right
        };

        static constexpr std::array<Direction, 4> DIRECTIONS = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};

        /// The `<direction>_` prefix a Direction corresponds to (e.g. Direction::Down -> "down"),
        /// needed only to build/parse the (string-keyed) SpriteState lookup name.
        static std::string direction_prefix(Direction direction);

        /// Splits a state name like "down_walking" into {Direction::Down, "walking"} if it starts
        /// with one of DIRECTIONS' prefixes followed by '_'; std::nullopt if the name doesn't
        /// follow the convention (e.g. a sprite class with only one, undirected, state).
        static std::optional<std::pair<Direction, std::string>> split_state_name(const std::string &state_name);

        /// Maps a (non-zero) heading to the closest Direction, by whichever axis dominates.
        static Direction direction_from_heading(const glm::vec2 &heading);

        /// Inverse of direction_from_heading(), for initializing Facing to the one direction a
        /// SpriteOrientation entity is spawned already showing.
        static glm::vec2 direction_vector(Direction direction);

        /// If `sprite` defines all four Directions of `initial_state_name`'s action (per
        /// split_state_name()), emplaces Facing (initialized to the direction `initial_state_name`
        /// already shows) and SpriteOrientation on `entity`, so systems::SpriteOrientation can
        /// reorient it at runtime as its Facing changes. Otherwise a no-op - e.g. for a
        /// single-orientation sprite, or a state name that doesn't follow the convention at all.
        /// Used by both systems::Script (via `_make_orientable_if_directional`, for map-loaded
        /// sprite entities) and systems::Player (for the keyboard-controlled player, which
        /// follows the same "man" class/state convention).
        static void make_orientable_if_directional(entt::registry &registry, entt::entity entity, const engine::graphics::Sprite &sprite, const std::string &initial_state_name);

        const engine::graphics::Sprite *sprite;
        std::string action;
        /// Direction last applied, so systems::SpriteOrientation can skip redundant
        /// lookups/patches once Facing hasn't crossed into a new cardinal direction.
        Direction current_direction;
    };
} // namespace tilegame::components
