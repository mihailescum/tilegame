#include "messagebox.hpp"

#include <sstream>
#include <algorithm>

#include "components/messagebox.hpp"
#include "components/player.hpp"
#include "components/inactive.hpp"
#include "components/event.hpp"

namespace tilegame::systems
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
    }

    MessageBox::MessageBox(tilegame::Scene &scene, entt::registry &registry)
        : System(scene, registry), _enter_was_down(false), _up_was_down(false), _down_was_down(false)
    {
    }

    void MessageBox::initialize()
    {
        _registry.ctx().emplace<components::MessageBoxState>();

        const auto entity = _registry.create();
        _registry.emplace<components::EventListener<components::ShowMessageEvent>>(
            entity,
            [this](const std::string &, const components::ShowMessageEvent &event, entt::entity)
            { on_show_message(event); },
            entt::null);
    }

    std::size_t MessageBox::max_line_chars() const
    {
        const auto &font = _scene.game().resource_manager().get<engine::graphics::SpriteFont>("font_default");
        const auto &viewport = _scene.game().graphicsdevice().viewport();
        const int usable_width = viewport.dimensions.x - static_cast<int>(2 * messagebox_layout::BOX_PADDING);
        return std::max(1, usable_width / font.cell_width());
    }

    void MessageBox::on_show_message(const components::ShowMessageEvent &event)
    {
        auto &state = _registry.ctx().get<components::MessageBoxState>();
        const auto lines = wrap_text(event.text, max_line_chars());

        if (event.append && !state.lines.empty())
        {
            state.lines.insert(state.lines.end(), lines.begin(), lines.end());
        }
        else
        {
            state.lines = lines;
            raise_event<components::MessageOpenedEvent>();
        }

        state.options = event.options;
        state.selected_option = 0;
        state.showing_options = false;
    }

    void MessageBox::update(const engine::GameTime &update_time)
    {
        const auto &window = _scene.game().window();
        auto &state = _registry.ctx().get<components::MessageBoxState>();

        const bool up_is_down = window.is_key_pressed(GLFW_KEY_UP);
        const bool up_pressed = up_is_down && !_up_was_down;
        _up_was_down = up_is_down;

        const bool down_is_down = window.is_key_pressed(GLFW_KEY_DOWN);
        const bool down_pressed = down_is_down && !_down_was_down;
        _down_was_down = down_is_down;

        // Once the options box is showing, Up/Down cycle the highlighted option.
        if (state.showing_options)
        {
            if (up_pressed)
            {
                state.selected_option = (state.selected_option + state.options.size() - 1) % state.options.size();
            }
            if (down_pressed)
            {
                state.selected_option = (state.selected_option + 1) % state.options.size();
            }
        }

        const bool enter_is_down = window.is_key_pressed(GLFW_KEY_ENTER);
        const bool enter_pressed = enter_is_down && !_enter_was_down;
        _enter_was_down = enter_is_down;

        if (enter_pressed)
        {
            if (state.showing_options)
            {
                // Confirm the highlighted option (currently a no-op beyond closing and reporting
                // it on the MessageClosedEvent - nothing reacts to it yet) and close both boxes
                // together.
                const std::string selected_option = state.options[state.selected_option];

                state.lines.clear();
                state.options.clear();
                state.selected_option = 0;
                state.showing_options = false;

                raise_event<components::MessageClosedEvent>(entt::null, selected_option);
            }
            else if (!state.options.empty() && state.lines.size() <= static_cast<std::size_t>(messagebox_layout::VISIBLE_LINES))
            {
                // The last line is already on screen: reveal the options box instead of
                // dismissing the message, and leave `lines` untouched so it doesn't scroll.
                state.showing_options = true;
            }
            else if (!state.lines.empty())
            {
                state.lines.pop_front();

                if (state.lines.empty())
                {
                    raise_event<components::MessageClosedEvent>();
                }
            }
        }
    }
} // namespace tilegame::systems
