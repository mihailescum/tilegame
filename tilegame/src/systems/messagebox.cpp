#include "messagebox.hpp"

#include <sstream>
#include <algorithm>

#include "components/messagebox.hpp"
#include "components/player.hpp"
#include "components/inactive.hpp"

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
        : System(scene, registry), _enter_was_down(false)
    {
    }

    void MessageBox::initialize()
    {
        _registry.ctx().emplace<components::MessageBoxState>();
    }

    std::size_t MessageBox::max_line_chars() const
    {
        const auto &font = _scene.game().resource_manager().get<engine::graphics::SpriteFont>("font_default");
        const auto &viewport = _scene.game().graphicsdevice().viewport();
        const int usable_width = viewport.dimensions.x - static_cast<int>(2 * messagebox_layout::BOX_PADDING);
        return std::max(1, usable_width / font.cell_width());
    }

    void MessageBox::apply_pending_events()
    {
        for (auto &&[entity, event] : _registry.view<components::ShowMessageEvent>().each())
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
                const auto opened_entity = _registry.create();
                _registry.emplace<components::MessageOpenedEvent>(opened_entity);
                raise_events<components::MessageOpenedEvent>();
                _registry.destroy(opened_entity);
            }

            _registry.destroy(entity);
        }
    }

    void MessageBox::update(const engine::GameTime &update_time)
    {
        apply_pending_events();

        const auto &window = _scene.game().window();
        const bool enter_is_down = window.is_key_pressed(GLFW_KEY_ENTER);
        const bool enter_pressed = enter_is_down && !_enter_was_down;
        _enter_was_down = enter_is_down;

        if (enter_pressed)
        {
            auto &state = _registry.ctx().get<components::MessageBoxState>();
            if (!state.lines.empty())
            {
                state.lines.pop_front();

                if (state.lines.empty())
                {
                    const auto closed_entity = _registry.create();
                    _registry.emplace<components::MessageClosedEvent>(closed_entity);
                    raise_events<components::MessageClosedEvent>();
                    _registry.destroy(closed_entity);
                }
            }
        }
    }
} // namespace tilegame::systems
