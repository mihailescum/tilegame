#include "script.hpp"

#include "entt_sol/bond.hpp"

#include "components/scriptloader.hpp"
#include "components/timer.hpp"
#include "components/luatable.hpp"
#include "components/inactive.hpp"
#include "components/target.hpp"
#include "components/speed.hpp"
#include "components/transform.hpp"
#include "components/pin.hpp"
#include "components/direction.hpp"
#include "components/currentmap.hpp"

#define AUTO_ARG(x) decltype(x), x

namespace tilegame::systems
{
    Script::Script(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    Script::~Script()
    {
        // Clear all components before the lua state is destroyed to avoid invalid references which would lead to a SegFault
        _registry.clear();
    }

    void Script::initialize()
    {
        _lua.open_libraries();
        register_api();
    }

    void Script::register_api()
    {
        _lua().require("_registry", sol::c_call<AUTO_ARG(&entt_sol::open_registry)>, false);
        _lua()["_registry"] = std::ref(_registry);

        // Define the user types
        _lua().new_usertype<glm::vec2>(
            "vec2",
            sol::call_constructor,
            sol::factories(
                []()
                { return glm::vec2(0.0); },
                [](float x, float y)
                { return glm::vec2(x, y); }),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y);
        _lua().new_usertype<entt::entity>(
            "_entity", sol::no_constructor);

        components::Direction::register_component(_lua());
        components::Inactive::register_component(_lua());
        components::LuaTable::register_component(_lua());
        components::MapEnteredEvent::register_component(_lua());
        components::MapLeftEvent::register_component(_lua());
        components::Pin::register_component(_lua());
        components::ScriptLoader::register_component(_lua());
        components::Target::register_component(_lua());
        components::TargetReachedEvent::register_component(_lua());
        components::Timer::register_component(_lua());
        components::TimerEvent::register_component(_lua());
        components::Transform::register_component(_lua());
        components::Speed::register_component(_lua());

        _lua().set_function("_add_event_listener",
                            sol::overload(
                                [this](const sol::table &event, sol::function callback)
                                { return Script::add_event_listener(event, callback, entt::null); },
                                [this](const sol::table &event, sol::function callback, entt::entity source)
                                { return Script::add_event_listener(event, callback, source); }));
        _lua().set_function("_to_global", sol::resolve<glm::vec2(const std::string &, const glm::vec2 &) const>(&Script::to_global), this);
        register_event_type<components::TargetReachedEvent, components::EventListener<components::TargetReachedEvent>>();
        register_event_type<components::TimerEvent, components::EventListener<components::TimerEvent>>();
        register_event_type<components::MapEnteredEvent, components::EventListener<components::MapEnteredEvent>>();
        register_event_type<components::MapLeftEvent, components::EventListener<components::MapLeftEvent>>();
    }

    bool Script::add_event_listener(const sol::table &event, sol::function callback, entt::entity source)
    {
        auto event_type = event["EVENT_TYPE"];
        if (event_type.valid() && _event_types.find(event_type) != _event_types.end())
        {
            return _event_types[event_type](callback, source);
        }
        else
        {
            return false;
        }
    }

    glm::vec2 Script::to_global(const std::string &map_name, const glm::vec2 &relative_position) const
    {
        const auto &world = _scene.game().resource_manager().get<engine::tilemap::World>("world1");
        return world.to_global(map_name, relative_position);
    }

    void Script::update(const engine::GameTime &update_time)
    {
        // TODO loading scripts should happen somewhere else, not on update

        const auto script_entities = _registry.view<const components::ScriptLoader>(entt::exclude<components::Inactive>);
        for (const auto &&[entity, script_loader] : script_entities.each())
        {
            const auto &script_path = script_loader.path;
            sol::load_result load_result = _lua().load_file(script_path);
            if (load_result.valid())
            {
                sol::table arg = _lua().create_table_with("entity", entity);
                sol::table result = load_result(arg);
                _registry.emplace<components::LuaTable>(entity, result);
            }
            else
            {
                throw "Error loading file";
            }

            _registry.erase<components::ScriptLoader>(entity);
        }
    }
} // namespace tilegame::systems
