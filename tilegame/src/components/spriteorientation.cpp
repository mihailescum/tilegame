#include "spriteorientation.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

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
            throw std::runtime_error("Unknown Direction");
        }
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
            throw std::runtime_error("Unknown Direction");
        }
    }

    void SpriteOrientation::make_orientable_if_directional(entt::registry &registry, entt::entity entity, const engine::graphics::Sprite &sprite, Direction initial_direction, const std::string &action)
    {
        const bool has_all_directions = std::all_of(
            DIRECTIONS.begin(), DIRECTIONS.end(),
            [&](Direction direction)
            { return sprite.has_state(direction_prefix(direction) + "_" + action); });

        if (!has_all_directions)
            return;

        registry.emplace<components::Facing>(entity, direction_vector(initial_direction));
        registry.emplace<components::SpriteOrientation>(entity, &sprite, action, initial_direction);
    }

    void SpriteOrientation::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<SpriteOrientation>();

        lua.new_usertype<SpriteOrientation>(
            "_SpriteOrientation",
            "type_id", &entt::type_hash<SpriteOrientation>::value,
            sol::call_constructor,
            sol::factories(
                [](const engine::graphics::Sprite &sprite, const std::string &action, const glm::vec2 &heading)
                { return SpriteOrientation{&sprite, action, direction_from_heading(heading)}; }));
    }
} // namespace tilegame::components
