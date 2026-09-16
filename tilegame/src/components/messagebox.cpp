#include "messagebox.hpp"

#include <sstream>

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string MessageClosedEvent::to_string() const
    {
        std::stringstream ss;
        ss << "MessageClosedEvent(selected_option=" << selected_option << ")";
        return ss.str();
    }

    void MessageClosedEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<MessageClosedEvent>();

        lua.new_usertype<MessageClosedEvent>(
            "_MessageClosedEvent",
            "type_id", &entt::type_hash<MessageClosedEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return MessageClosedEvent(); },
                [](const std::string &selected_option)
                { return MessageClosedEvent(selected_option); }),
            "selected_option", &MessageClosedEvent::selected_option,
            "EVENT_TYPE", sol::var(MessageClosedEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &MessageClosedEvent::to_string);
    }

    std::string MessageOpenedEvent::to_string() const
    {
        std::stringstream ss;
        ss << "MessageOpenedEvent";
        return ss.str();
    }

    void MessageOpenedEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<MessageOpenedEvent>();

        lua.new_usertype<MessageOpenedEvent>(
            "_MessageOpenedEvent",
            "type_id", &entt::type_hash<MessageOpenedEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return MessageOpenedEvent(); }),
            "EVENT_TYPE", sol::var(MessageOpenedEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &MessageOpenedEvent::to_string);
    }
} // namespace tilegame::components
