#pragma once

#include <cstddef>

#include "engine.hpp"

#include "system.hpp"
#include "components/messagebox.hpp"

namespace tilegame::systems
{
    /**
     * @brief Drives dialog messages requested via components::ShowMessageEvent.
     *
     * Owns the current message state as a single components::MessageBoxState value in the
     * registry's ctx() storage (created in initialize()) - not an entity/component, so
     * systems::Render can still read it to know what to draw without this system needing to
     * expose anything directly to it. Each frame, first consumes every pending
     * components::ShowMessageEvent (raised by the `_show_message` Lua binding on its own
     * throwaway entity): word-wraps its text and either appends the result to the state or
     * replaces it, depending on the event's `append` flag, raising a MessageOpenedEvent the
     * moment a message becomes active. Then pops a line off the front on each Enter key press
     * until none are left, at which point it raises a MessageClosedEvent.
     */
    class MessageBox : public System
    {
    private:
        bool _enter_was_down;

        void apply_pending_events();
        // Max characters per line the on-screen dialog box can fit, from the current viewport
        // width and font.
        std::size_t max_line_chars() const;

    public:
        MessageBox(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
