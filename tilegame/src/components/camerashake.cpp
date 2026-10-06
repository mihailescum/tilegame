#include "camerashake.hpp"

#include <sstream>

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string CameraShakeEndedEvent::to_string() const
    {
        std::stringstream ss;
        ss << "CameraShakeEndedEvent";
        return ss.str();
    }

    void CameraShakeEndedEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<CameraShakeEndedEvent>();

        lua.new_usertype<CameraShakeEndedEvent>(
            "_CameraShakeEndedEvent",
            "type_id", &entt::type_hash<CameraShakeEndedEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return CameraShakeEndedEvent(); }),
            "EVENT_TYPE", sol::var(CameraShakeEndedEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &CameraShakeEndedEvent::to_string);
    }
} // namespace tilegame::components
