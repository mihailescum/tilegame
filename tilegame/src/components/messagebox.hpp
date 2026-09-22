#pragma once

#include <string>
#include <vector>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Raised via System::raise() by the `_show_message` Lua binding, immediately
     * delivered to systems::MessageBox's EventListener<ShowMessageEvent> (registered in
     * MessageBox::initialize()), which word-wraps `text` and either appends the result to the
     * currently displayed message or replaces it, depending on `append`. Not exposed to Lua as
     * a usertype - nothing outside systems::MessageBox subscribes to it today - but carries
     * EVENT_TYPE like any other event since System::raise_event() needs it regardless.
     */
    struct ShowMessageEvent
    {
        inline static const std::string EVENT_TYPE = "SHOW_MESSAGE_EVENT";

        /// Raw message text; may contain '\n' for explicit line breaks.
        std::string text;
        /// If true and a message is currently displayed, `text`'s wrapped lines are appended to
        /// it instead of replacing it.
        bool append = false;
        /// If non-empty, shown as a selectable list alongside the message, replacing whatever
        /// options (if any) were showing before. Empty means no options box.
        std::vector<std::string> options;
    };

    /**
     * @brief Whether the UI message box (scenes::UiScene) is currently open. Not an entity/
     * component - a single value held in the entt::registry's ctx() storage (see
     * entt::basic_registry::ctx()), which every system already has access to through the
     * registry reference it's constructed with. The message's actual content and rendering live
     * entirely in scenes::UiScene now, not the registry; this is the one bit systems::Interaction
     * still needs (to avoid triggering a new interaction while a message is on screen), set by
     * systems::MessageBox whenever it pushes/is notified that scenes::UiScene has closed.
     */
    struct MessageBoxOpenState
    {
        bool open = false;
    };

    /**
     * @brief Entt event component raised for one frame by systems::MessageBox when a message has been
     * fully dismissed (its last line consumed), and delivered to `EventListener<MessageClosedEvent>`
     * components via the engine's event-listener mechanism before being cleared. Exposed to Lua as
     * `_MessageClosedEvent`.
     */
    struct MessageClosedEvent
    {
        inline static const std::string EVENT_TYPE = "MESSAGE_CLOSED_EVENT";

        /// Text of the option that was highlighted when the message closed; empty if the
        /// message had no options attached.
        std::string selected_option;

        MessageClosedEvent() = default;
        MessageClosedEvent(const std::string &selected_option) : selected_option(selected_option) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    struct MessageOpenedEvent
    {
        inline static const std::string EVENT_TYPE = "MESSAGE_OPENED_EVENT";

        MessageOpenedEvent() = default;
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
