#include "scenes/uiscene.hpp"

#include <algorithm>
#include <sstream>

namespace tilegame::scenes
{
    namespace
    {
        // Splits `text` on '\n' into paragraphs, then greedily packs each paragraph's words into
        // lines no wider than `max_line_chars`, hard-splitting any single word that's longer than
        // that on its own. Blank paragraphs (consecutive '\n') are preserved as empty lines.
        std::deque<std::string> wrap_text(const std::string &text, std::size_t max_line_chars)
        {
            max_line_chars = std::max<std::size_t>(max_line_chars, 1);

            std::deque<std::string> lines;
            std::size_t paragraph_start = 0;
            while (true)
            {
                const std::size_t newline_pos = text.find('\n', paragraph_start);
                const std::string paragraph = text.substr(paragraph_start, newline_pos == std::string::npos ? std::string::npos : newline_pos - paragraph_start);

                std::string current_line;
                std::istringstream words_stream(paragraph);
                std::string word;
                while (words_stream >> word)
                {
                    while (word.size() > max_line_chars)
                    {
                        if (!current_line.empty())
                        {
                            lines.push_back(current_line);
                            current_line.clear();
                        }
                        lines.push_back(word.substr(0, max_line_chars));
                        word = word.substr(max_line_chars);
                    }

                    const std::size_t needed = current_line.empty() ? word.size() : current_line.size() + 1 + word.size();
                    if (needed > max_line_chars)
                    {
                        lines.push_back(current_line);
                        current_line = word;
                    }
                    else
                    {
                        if (!current_line.empty())
                        {
                            current_line += ' ';
                        }
                        current_line += word;
                    }
                }
                lines.push_back(current_line);

                if (newline_pos == std::string::npos)
                {
                    break;
                }
                paragraph_start = newline_pos + 1;
            }

            return lines;
        }
    } // namespace

    UIScene::UIScene(engine::Game &game)
        : engine::Scene(game), _spritebatch(game.graphicsdevice())
    {
    }

    void UIScene::initialize()
    {
        _spritebatch.create();
    }

    void UIScene::load_content()
    {
        _font = game().resource_manager().load_resource<engine::graphics::SpriteFont>("font_default", "content/fonts/default.json");
        _rect_texture = game().resource_manager().load_resource<engine::Texture2D>("white_rect", "content/textures/white_rect.png");
    }

    std::size_t UIScene::max_line_chars() const
    {
        const auto &viewport = game().graphicsdevice().viewport();
        const int usable_width = viewport.dimensions.x - static_cast<int>(2 * BOX_PADDING);
        return std::max(1, usable_width / _font->cell_width());
    }

    bool UIScene::show_message(const std::string &text, bool append, const std::vector<std::string> &options)
    {
        const auto lines = wrap_text(text, max_line_chars());
        const bool opened = !(append && !_lines.empty());

        if (append && !_lines.empty())
        {
            _lines.insert(_lines.end(), lines.begin(), lines.end());
        }
        else
        {
            _lines = lines;
        }

        _options = options;
        _selected_option = 0;
        _showing_options = false;

        return opened;
    }

    void UIScene::update(const engine::GameTime &update_time)
    {
        const auto &window = game().window();

        const bool up_is_down = window.is_key_pressed(GLFW_KEY_UP);
        const bool up_pressed = up_is_down && !_up_was_down;
        _up_was_down = up_is_down;

        const bool down_is_down = window.is_key_pressed(GLFW_KEY_DOWN);
        const bool down_pressed = down_is_down && !_down_was_down;
        _down_was_down = down_is_down;

        // Once the options box is showing, Up/Down cycle the highlighted option.
        if (_showing_options)
        {
            if (up_pressed)
            {
                _selected_option = (_selected_option + _options.size() - 1) % _options.size();
            }
            if (down_pressed)
            {
                _selected_option = (_selected_option + 1) % _options.size();
            }
        }

        const bool enter_is_down = window.is_key_pressed(GLFW_KEY_ENTER);
        const bool enter_pressed = enter_is_down && !_enter_was_down;
        _enter_was_down = enter_is_down;

        if (!enter_pressed)
        {
            return;
        }

        if (_showing_options)
        {
            // Confirm the highlighted option and close, delivering it to whoever pushed this scene.
            close(_options[_selected_option]);
        }
        else if (!_options.empty() && _lines.size() <= static_cast<std::size_t>(VISIBLE_LINES))
        {
            // The last line is already on screen: reveal the options box instead of dismissing
            // the message, and leave `_lines` untouched so it doesn't scroll.
            _showing_options = true;
        }
        else if (!_lines.empty())
        {
            _lines.pop_front();

            if (_lines.empty())
            {
                close();
            }
        }
    }

    void UIScene::draw(const engine::GameTime &draw_time)
    {
        if (_lines.empty())
        {
            return;
        }

        draw_message_box();

        if (_showing_options)
        {
            draw_options_box();
        }
    }

    void UIScene::draw_message_box()
    {
        const auto &viewport = game().graphicsdevice().viewport();
        const int cell_height = _font->cell_height();

        const float box_width = static_cast<float>(viewport.dimensions.x);
        const float box_height = BOX_PADDING * 2 + cell_height * VISIBLE_LINES;
        const float box_y = viewport.dimensions.y - BOX_MARGIN_BOTTOM - box_height;

        _spritebatch.begin(true);

        const engine::Rectangle box_rect(glm::vec2(0.0f, box_y), glm::vec2(box_width, box_height));
        _spritebatch.draw(*_rect_texture, box_rect, nullptr, engine::Color(0.0f, 0.0f, 0.0f, 0.75f));

        int line_index = 0;
        for (const auto &line : _lines)
        {
            if (line_index >= VISIBLE_LINES)
            {
                break;
            }

            const float line_bottom = box_y + BOX_PADDING + (line_index + 1) * cell_height;
            _spritebatch.draw_text(line, *_font, glm::vec2(BOX_PADDING, line_bottom), engine::Color::WHITE);

            line_index++;
        }

        _spritebatch.end();
    }

    void UIScene::draw_options_box()
    {
        const auto &viewport = game().graphicsdevice().viewport();
        const int cell_height = _font->cell_height();

        const float message_box_height = BOX_PADDING * 2 + cell_height * VISIBLE_LINES;
        const float message_box_y = viewport.dimensions.y - BOX_MARGIN_BOTTOM - message_box_height;

        const float box_width = viewport.dimensions.x * OPTIONS_BOX_WIDTH_RATIO;
        const float box_height = BOX_PADDING * 2 + cell_height * static_cast<float>(_options.size());
        const float box_x = viewport.dimensions.x - box_width;
        const float box_y = message_box_y - OPTIONS_BOX_MARGIN_BOTTOM - box_height;

        _spritebatch.begin(true);

        const engine::Rectangle box_rect(glm::vec2(box_x, box_y), glm::vec2(box_width, box_height));
        _spritebatch.draw(*_rect_texture, box_rect, nullptr, engine::Color(0.0f, 0.0f, 0.0f, 0.75f));

        for (std::size_t option_index = 0; option_index < _options.size(); option_index++)
        {
            const float line_bottom = box_y + BOX_PADDING + (option_index + 1) * cell_height;
            const std::string prefix = option_index == _selected_option ? "> " : "  ";
            _spritebatch.draw_text(prefix + _options[option_index], *_font, glm::vec2(box_x + BOX_PADDING, line_bottom), engine::Color::WHITE);
        }

        _spritebatch.end();
    }
} // namespace tilegame::scenes
