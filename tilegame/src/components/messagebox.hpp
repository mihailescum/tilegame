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
     * @brief Request/state for a dialog message. Created (with `text` set) by the `_show_message`
     * Lua binding; systems::MessageBox word-wraps `text` into `lines` the first time it sees the
     * component, then pops a line off the front each time the player presses Enter, destroying the
     * entity once `lines` is empty. systems::Render draws `lines[0]`/`lines[1]` (if present) as the
     * two visible lines of the on-screen box while this component exists. Not exposed to Lua as a
     * usertype, since scripts only ever create one via `_show_message`.
     */
    struct MessageBox
    {
        /// Raw message text, set once at creation; may contain '\n' for explicit line breaks.
        std::string text;
        /// Remaining word-wrapped lines still to be shown; populated by systems::MessageBox from `text`.
        std::deque<std::string> lines;

        MessageBox() = default;
        MessageBox(const std::string &text) : text(text) {}
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
