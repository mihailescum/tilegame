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
        : System(scene, registry), _active_entity(entt::null), _enter_was_down(false)
    {
    }

    void MessageBox::update(const engine::GameTime &update_time)
    {
        if (_active_entity == entt::null)
        {
            const auto pending = _registry.view<components::MessageBox>();
            if (!pending.empty())
            {
                const auto entity = *pending.begin();
                auto &box = pending.get<components::MessageBox>(entity);

                const auto &font = _scene.game().resource_manager().get<engine::graphics::SpriteFont>("font_default");
                const auto &viewport = _scene.game().graphicsdevice().viewport();
                const int usable_width = viewport.dimensions.x - static_cast<int>(2 * messagebox_layout::BOX_PADDING);
                const std::size_t max_line_chars = std::max(1, usable_width / font.cell_width());

                box.lines = wrap_text(box.text, max_line_chars);
                _active_entity = entity;

                _registry.emplace<components::MessageOpenedEvent>(_active_entity);
                raise_events<components::MessageOpenedEvent>();
            }
        }

        const auto &window = _scene.game().window();
        const bool enter_is_down = window.is_key_pressed(GLFW_KEY_ENTER);
        const bool enter_pressed = enter_is_down && !_enter_was_down;
        _enter_was_down = enter_is_down;

        if (_active_entity != entt::null && enter_pressed)
        {
            auto &box = _registry.get<components::MessageBox>(_active_entity);
            if (!box.lines.empty())
            {
                box.lines.pop_front();
            }

            if (box.lines.empty())
            {
                _registry.emplace<components::MessageClosedEvent>(_active_entity);
                raise_events<components::MessageClosedEvent>();

                _registry.destroy(_active_entity);
                _active_entity = entt::null;
            }
        }
    }

    void MessageBox::end_update()
    {
        _registry.clear<components::MessageClosedEvent>();
    }
} // namespace tilegame::systems
