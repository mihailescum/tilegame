#include "messagebox.hpp"

#include "components/messagebox.hpp"
#include "components/event.hpp"

namespace tilegame::systems
{
    MessageBox::MessageBox(tilegame::Scene &scene, entt::registry &registry)
        : System(scene, registry)
    {
    }

    void MessageBox::initialize()
    {
        _registry.ctx().emplace<components::MessageBoxOpenState>();

        const auto entity = _registry.create();
        _registry.emplace<components::EventListener<components::ShowMessageEvent>>(
            entity,
            [this](const std::string &, const components::ShowMessageEvent &event, entt::entity, entt::entity)
            { on_show_message(event); },
            entt::null);
    }

    void MessageBox::on_show_message(const components::ShowMessageEvent &event)
    {
        if (!_ui_scene)
        {
            _ui_scene = &_scene.game().scene_manager().push_for_result<scenes::UIScene>(
                [this](std::any result)
                { on_message_closed(std::move(result)); });
            _registry.ctx().get<components::MessageBoxOpenState>().open = true;
        }

        if (_ui_scene->show_message(event.text, event.append, event.options))
        {
            raise_event<components::MessageOpenedEvent>();
        }
    }

    void MessageBox::on_message_closed(std::any result)
    {
        _ui_scene = nullptr;
        _registry.ctx().get<components::MessageBoxOpenState>().open = false;

        const std::string selected_option = result.has_value() ? std::any_cast<std::string>(result) : std::string();
        raise_event<components::MessageClosedEvent>(entt::null, entt::null, selected_option);
    }
} // namespace tilegame::systems
