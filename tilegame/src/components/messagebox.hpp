#pragma once

#include <string>
#include <deque>
#include <vector>
#include <cstddef>

#include "sol/sol.hpp"

namespace tilegame::messagebox_layout
{
    /// Layout of the on-screen dialog box, in screen pixels; shared by systems::MessageBox (to
    /// compute word-wrap width) and systems::Render (to actually draw the box), so the two stay in sync.
    inline constexpr float BOX_PADDING = 16.0f;
    inline constexpr float BOX_MARGIN_BOTTOM = 24.0f;
    inline constexpr int VISIBLE_LINES = 2;
    /// Width of the options box, as a fraction of the viewport width.
    inline constexpr float OPTIONS_BOX_WIDTH_RATIO = 0.15f;
    /// Gap between the bottom of the options box and the top of the dialog box above which it sits.
    inline constexpr float OPTIONS_BOX_MARGIN_BOTTOM = 8.0f;
} // namespace tilegame::messagebox_layout

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
     * @brief The currently displayed dialog message. Not an entity/component - a single value
     * held in the entt::registry's ctx() storage (see entt::basic_registry::ctx()), which every
     * system already has access to through the registry reference it's constructed with, so no
     * system needs a direct reference to another one to reach it. Created once, in
     * systems::MessageBox::initialize(), and is the only state that system mutates in place as
     * it consumes ShowMessageEvents and Enter/Up/Down key presses. `lines` empty means no
     * message is currently displayed. systems::Render reads it
     * (_registry.ctx().get<MessageBoxState>()) each draw() to know whether/what to draw; it
     * never writes it.
     */
    struct MessageBoxState
    {
        /// Remaining word-wrapped lines still to be shown; empty means nothing is displayed.
        std::deque<std::string> lines;
        /// Selectable options attached to the message; empty means no options box. Set from
        /// ShowMessageEvent::options, but not shown/interactable until `showing_options` is
        /// true. Cleared, along with `selected_option` and `showing_options`, whenever the
        /// message closes.
        std::vector<std::string> options;
        /// Index into `options` currently highlighted; cycled by Up/Down while `showing_options`
        /// is true.
        std::size_t selected_option = 0;
        /// True once Enter has been pressed while the message's last line was already on
        /// screen (i.e. no more pages left in `lines`) and `options` is non-empty: reveals the
        /// options box in place of dismissing the message, and freezes `lines` so the last line
        /// doesn't scroll. While true, a further Enter press closes the message and options box
        /// together instead of popping `lines`.
        bool showing_options = false;
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
