#include "lightning.hpp"

#include <sstream>

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string LightningEvent::to_string() const
    {
        std::stringstream ss;
        ss << "LightningEvent";
        return ss.str();
    }

    void LightningEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<LightningEvent>();

        lua.new_usertype<LightningEvent>(
            "_LightningEvent",
            "type_id", &entt::type_hash<LightningEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return LightningEvent(); }),
            "EVENT_TYPE", sol::var(LightningEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &LightningEvent::to_string);
    }
} // namespace tilegame::components
