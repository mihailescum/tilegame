#pragma once

#include <string>
#include <optional>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief World-space destination for an entity to walk toward. Adding this component to an
     * entity with a Transform makes the MovementController system attach Direction/Movement
     * components and steer it toward `target`; the Movement system removes Target (along with
     * Movement and Speed) and raises a TargetReachedEvent once the destination is reached. Exposed
     * to Lua as `_Target`.
     */
    struct Target
    {
        glm::vec2 target;
        /// Position the entity started moving from; auto-filled from its Transform if not given explicitly.
        std::optional<glm::vec2> start;

        Target() : Target(glm::vec2(0.0)) {}
        Target(const glm::vec2 &target) : target(target) {}
        Target(const glm::vec2 &target, const glm::vec2 &start) : target(target), start(start) {}

        const glm::vec2 &operator()() const { return target; }
        glm::vec2 &operator()() { return target; }
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    /**
     * @brief Entt event component raised for one frame by the Movement system when an entity's
     * Target is reached, and delivered to `EventListener<TargetReachedEvent>` components via the
     * engine's event-listener mechanism before being cleared. Exposed to Lua as `_TargetReachedEvent`.
     */
    struct TargetReachedEvent
    {
        inline static const std::string EVENT_TYPE = "TARGET_REACHED_EVENT";
        glm::vec2 target;

        TargetReachedEvent() {}
        TargetReachedEvent(const glm::vec2 &target) : target(target) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
