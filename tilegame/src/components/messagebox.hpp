#pragma once

#include <string>
#include <deque>

#include "sol/sol.hpp"

namespace tilegame::messagebox_layout
{
    /// Layout of the on-screen dialog box, in screen pixels; shared by systems::MessageBox (to
    /// compute word-wrap width) and systems::Render (to actually draw the box), so the two stay in sync.
    inline constexpr float BOX_PADDING = 16.0f;
    inline constexpr float BOX_MARGIN_BOTTOM = 24.0f;
    inline constexpr int VISIBLE_LINES = 2;
} // namespace tilegame::messagebox_layout

namespace tilegame::components
{
    /**
     * @brief Raised on a fresh entity by the `_show_message` Lua binding; consumed and
     * destroyed by systems::MessageBox the next time it updates, which word-wraps `text` and
     * either appends the result to the currently displayed message or replaces it, depending
     * on `append`. systems::MessageBox is this event's only subscriber, so it skips the
     * generic EventListener<T>/raise_events() broadcast machinery. Not exposed to Lua as a
     * usertype: scripts only ever raise one via `_show_message`.
     */
    struct ShowMessageEvent
    {
        /// Raw message text; may contain '\n' for explicit line breaks.
        std::string text;
        /// If true and a message is currently displayed, `text`'s wrapped lines are appended to
        /// it instead of replacing it.
        bool append = false;
    };

    /**
     * @brief The currently displayed dialog message. Not an entity/component - a single value
     * held in the entt::registry's ctx() storage (see entt::basic_registry::ctx()), which every
     * system already has access to through the registry reference it's constructed with, so no
     * system needs a direct reference to another one to reach it. Created once, in
     * systems::MessageBox::initialize(), and is the only state that system mutates in place as
     * it consumes ShowMessageEvents and Enter key presses. `lines` empty means no message is
     * currently displayed. systems::Render reads it (_registry.ctx().get<MessageBoxState>())
     * each draw() to know whether/what to draw; it never writes it.
     */
    struct MessageBoxState
    {
        /// Remaining word-wrapped lines still to be shown; empty means nothing is displayed.
        std::deque<std::string> lines;
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

        MessageClosedEvent() = default;
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
