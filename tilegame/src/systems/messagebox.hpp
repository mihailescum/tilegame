#pragma once

#include <any>

#include "engine.hpp"

#include "system.hpp"
#include "components/messagebox.hpp"
#include "scenes/uiscene.hpp"

namespace tilegame::systems
{
    /**
     * @brief Bridges `_show_message` Lua calls into scenes::UIScene.
     *
     * Owns no message state itself - that lives entirely in scenes::UIScene, which is pushed on
     * top of the current scene stack (via the SceneManager) the first time a
     * components::ShowMessageEvent arrives with none already open, and reused for any further
     * calls (append or replace) while it's still up. In initialize(), creates the
     * components::MessageBoxOpenState ctx() value (read by systems::Interaction to avoid
     * triggering a new interaction while a message is on screen) and registers an
     * EventListener<ShowMessageEvent> that reacts the instant the `_show_message` Lua binding
     * raises one via the inherited System::raise(), immediately delivering it to the open (or
     * newly pushed) UIScene, reaching the SceneManager via _scene.game().scene_manager(). Raises
     * a MessageOpenedEvent whenever that call counts as opening a new message (see
     * scenes::UIScene::show_message()), and a MessageClosedEvent - carrying whichever option was
     * selected, if any - once the UIScene closes itself and the SceneManager delivers its result
     * back via the close callback given to push_for_result().
     */
    class MessageBox : public System
    {
    private:
        // The currently open message box, or nullptr if none is up. Non-owning: the SceneManager
        // owns it, and clears this back to nullptr via on_message_closed() once it's popped.
        scenes::UIScene *_ui_scene = nullptr;

        // Applies one ShowMessageEvent to `_ui_scene`, pushing it first if none is open yet; the
        // body of the EventListener<ShowMessageEvent> registered in initialize().
        void on_show_message(const components::ShowMessageEvent &event);
        // The close callback given to push_for_result() when `_ui_scene` is pushed.
        void on_message_closed(std::any result);

    public:
        MessageBox(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
    };
} // namespace tilegame
