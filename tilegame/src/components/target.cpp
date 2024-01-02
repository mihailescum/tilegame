#include "target.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Target::to_string() const
    {
        std::stringstream ss;
        ss << "Target";
        return ss.str();
    }

    void Target::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Target>();

        lua.new_usertype<Target>(
            "_Target",
            "type_id", &entt::type_hash<Target>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Target(); },
                [](const glm::vec2 &target)
                { return Target(target); },
                [](const glm::vec2 &target, const glm::vec2 &start)
                { return Target(target, start); }),
            "target", &Target::target,
            "start", &Target::start,
            sol::meta_function::to_string, &Target::to_string);
    }

    std::string TargetReachedEvent::to_string() const
    {
        std::stringstream ss;
        ss << "TargetReachedEvent";
        return ss.str();
    }

    void TargetReachedEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<TargetReachedEvent>();

        lua.new_usertype<TargetReachedEvent>(
            "_TargetReachedEvent",
            "type_id", &entt::type_hash<TargetReachedEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return TargetReachedEvent(); },
                [](const glm::vec2 &target)
                { return TargetReachedEvent(target); }),
            "target", &components::TargetReachedEvent::target,
            "EVENT_TYPE", sol::var(TargetReachedEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &TargetReachedEvent::to_string);
    }
} // namespace tilegame::components
