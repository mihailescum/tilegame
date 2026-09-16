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
     * expose anything directly to it. Also in initialize(), registers an
     * EventListener<ShowMessageEvent> (on its own dedicated entity) that reacts the instant the
     * `_show_message` Lua binding raises one via the inherited System::raise(): word-wraps its
     * text and either appends the result to the state or replaces it, depending on the event's
     * `append` flag, raising a MessageOpenedEvent the moment a message becomes active; the
     * event's `options`, if any, replace the state's options list outright. Then, each frame,
     * pops a line off the front on each Enter key press - until either `lines` is exhausted (no
     * `options` attached: raises a MessageClosedEvent) or, once `lines` holds no more than one
     * page and `options` is non-empty, the *next* Enter instead reveals the options box
     * (`showing_options = true`) without popping, freezing the last line in place. While the
     * options box is showing, Up/Down cycle the highlighted option and a further Enter closes
     * both boxes together, clearing `lines`/`options`/`showing_options` and raising a
     * MessageClosedEvent; selecting one currently has no other effect (the return value isn't
     * surfaced anywhere yet).
     */
    class MessageBox : public System
    {
    private:
        bool _enter_was_down;
        bool _up_was_down;
        bool _down_was_down;

        // Applies one ShowMessageEvent to the current MessageBoxState; the body of the
        // EventListener<ShowMessageEvent> registered in initialize().
        void on_show_message(const components::ShowMessageEvent &event);
        // Max characters per line the on-screen dialog box can fit, from the current viewport
        // width and font.
        std::size_t max_line_chars() const;

    public:
        MessageBox(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
