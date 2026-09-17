#include "spriteorientation.hpp"

#include <algorithm>
#include <cmath>

#include "facing.hpp"

namespace tilegame::components
{
    std::string SpriteOrientation::direction_prefix(Direction direction)
    {
        switch (direction)
        {
        case Direction::Up:
            return "up";
        case Direction::Down:
            return "down";
        case Direction::Left:
            return "left";
        case Direction::Right:
            return "right";
        default:
            throw "Unknown Direction";
        }
    }

    std::optional<std::pair<SpriteOrientation::Direction, std::string>> SpriteOrientation::split_state_name(const std::string &state_name)
    {
        for (const Direction direction : DIRECTIONS)
        {
            const std::string prefix = direction_prefix(direction) + "_";
            if (state_name.compare(0, prefix.size(), prefix) == 0)
            {
                return std::make_pair(direction, state_name.substr(prefix.size()));
            }
        }
        return std::nullopt;
    }

    SpriteOrientation::Direction SpriteOrientation::direction_from_heading(const glm::vec2 &heading)
    {
        if (std::abs(heading.x) > std::abs(heading.y))
        {
            return heading.x > 0.0f ? Direction::Right : Direction::Left;
        }
        return heading.y > 0.0f ? Direction::Down : Direction::Up;
    }

    glm::vec2 SpriteOrientation::direction_vector(Direction direction)
    {
        switch (direction)
        {
        case Direction::Up:
            return glm::vec2(0.0f, -1.0f);
        case Direction::Down:
            return glm::vec2(0.0f, 1.0f);
        case Direction::Left:
            return glm::vec2(-1.0f, 0.0f);
        case Direction::Right:
            return glm::vec2(1.0f, 0.0f);
        default:
            throw "Unknown Direction";
        }
    }

    void SpriteOrientation::make_orientable_if_directional(entt::registry &registry, entt::entity entity, const engine::graphics::Sprite &sprite, const std::string &initial_state_name)
    {
        const auto parsed_state = split_state_name(initial_state_name);
        if (!parsed_state)
            return;

        const auto &[initial_direction, action] = *parsed_state;
        const bool has_all_directions = std::all_of(
            DIRECTIONS.begin(), DIRECTIONS.end(),
            [&](Direction direction)
            { return sprite.has_state(direction_prefix(direction) + "_" + action); });

        if (!has_all_directions)
            return;

        registry.emplace<components::Facing>(entity, direction_vector(initial_direction));
        registry.emplace<components::SpriteOrientation>(entity, &sprite, action, initial_direction);
    }
} // namespace tilegame::components
