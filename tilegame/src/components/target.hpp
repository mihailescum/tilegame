#pragma once

#include <string>
#include <optional>

#include <glm/glm.hpp>

namespace tilegame::components
{
    struct Target
    {
        glm::vec2 target;
        std::optional<glm::vec2> start;

        Target() : Target(glm::vec2(0.0)) {}
        Target(const glm::vec2 &target) : target(target) {}
        Target(const glm::vec2 &target, const glm::vec2 &start) : target(target), start(start) {}

        const glm::vec2 &operator()() const { return target; }
        glm::vec2 &operator()() { return target; }
    };

    struct TargetReachedEvent
    {
        inline static const std::string EVENT_TYPE = "TARGET_REACHED_EVENT";
        glm::vec2 target;

        TargetReachedEvent() {}
        TargetReachedEvent(const glm::vec2 &target) : target(target) {}
    };
} // namespace tilegame::components
