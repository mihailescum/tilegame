#pragma once

#include <cstddef>
#include <deque>
#include <string>
#include <vector>

#include "engine.hpp"

namespace tilegame::scenes
{
    /**
     * @brief Screen-space UI overlay scene: currently just the dialog/options message box.
     *
     * Deliberately plain C++ state - no entt::registry. A message box has no real need for an
     * ECS: at most one is on screen at a time, its state fits in a handful of members, and it's
     * pushed on top of whichever scene requested it via the SceneManager rather than living
     * inside that scene's own registry. Pushed by WorldScene's systems::MessageBox the first
     * time a `_show_message` Lua call arrives with none already open; subsequent calls while
     * this is still open go through show_message() again instead of pushing a second one.
     * Closes itself (see Scene::close()) once the message - and any options attached to
     * it - have been dismissed, delivering the selected option (or an empty std::any if there
     * were none) to the callback it was pushed with.
     */
    class UIScene : public engine::Scene
    {
    private:
        // Layout of the on-screen dialog/options boxes, in screen pixels. Private to this class
        // now that both the word-wrap width calculation and the actual drawing live here.
        static constexpr float BOX_PADDING = 16.0f;
        static constexpr float BOX_MARGIN_BOTTOM = 24.0f;
        static constexpr int VISIBLE_LINES = 2;
        // Width of the options box, as a fraction of the viewport width.
        static constexpr float OPTIONS_BOX_WIDTH_RATIO = 0.15f;
        // Gap between the bottom of the options box and the top of the dialog box above which it sits.
        static constexpr float OPTIONS_BOX_MARGIN_BOTTOM = 8.0f;

        engine::graphics::SpriteBatch<engine::Texture2D> _spritebatch;
        const engine::graphics::SpriteFont *_font = nullptr;
        const engine::Texture2D *_rect_texture = nullptr;

        // Remaining word-wrapped lines still to be shown; empty (once initial content has been
        // shown) means the message is fully dismissed.
        std::deque<std::string> _lines;
        // Selectable options attached to the message; empty means no options box. Not shown/
        // interactable until `_showing_options` is true.
        std::vector<std::string> _options;
        // Index into `_options` currently highlighted; cycled by Up/Down while `_showing_options`.
        std::size_t _selected_option = 0;
        // True once Enter has been pressed while the message's last line was already on screen
        // and `_options` is non-empty: reveals the options box in place of dismissing the
        // message, freezing `_lines` so the last line doesn't scroll. A further Enter then closes
        // the whole scene instead of popping `_lines`.
        bool _showing_options = false;

        bool _enter_was_down = false;
        bool _up_was_down = false;
        bool _down_was_down = false;

        // Max characters per line the on-screen dialog box can fit, from the current viewport
        // width and font.
        std::size_t max_line_chars() const;
        void draw_message_box();
        void draw_options_box();

    public:
        UIScene(engine::Game &game);

        void initialize() override;
        void load_content() override;
        void update(const engine::GameTime &update_time) override;
        void draw(const engine::GameTime &draw_time) override;

        // Applies one `_show_message` call: word-wraps `text` and either appends the result to
        // the message currently on screen or replaces it outright, depending on `append` -
        // appending only actually happens if a message is already showing, otherwise this always
        // behaves like a replace. `options`, if any, always replaces whatever options were
        // showing before (even when appending text), resetting the selection and hiding the
        // options box until Enter reveals it again. Returns true if this counts as opening a new
        // message (i.e. it did not append to an already non-empty one) - the caller uses that to
        // decide whether to raise components::MessageOpenedEvent.
        bool show_message(const std::string &text, bool append, const std::vector<std::string> &options);
    };
} // namespace tilegame::scenes
