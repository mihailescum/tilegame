#include "interactable.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Interactable::to_string() const
    {
        std::stringstream ss;
        ss << "Interactable";
        return ss.str();
    }

    void Interactable::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Interactable>();

        lua.new_usertype<Interactable>(
            "_Interactable",
            "type_id", &entt::type_hash<Interactable>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Interactable{}; }),
            sol::meta_function::to_string, &Interactable::to_string);
    }

    std::string InteractEvent::to_string() const
    {
        std::stringstream ss;
        ss << "InteractEvent";
        return ss.str();
    }

    void InteractEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<InteractEvent>();

        lua.new_usertype<InteractEvent>(
            "_InteractEvent",
            "type_id", &entt::type_hash<InteractEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return InteractEvent(); },
                [](entt::entity player)
                { return InteractEvent(player); }),
            "player", &InteractEvent::player,
            "EVENT_TYPE", sol::var(InteractEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &InteractEvent::to_string);
    }
} // namespace tilegame::components
