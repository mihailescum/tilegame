#include "sprite/sprite.hpp"

#include "sprite/spritesheet.hpp"

namespace engine::sprite
{
    /*void Sprite::parse(const pugi::xml_node &node)
    {
        _name = node.attribute("type").as_string();

        const auto state_node = node.child("properties").find_child_by_attribute("property", "name", "state");
        const std::string state_name = state_node.attribute("value").as_string();

        auto &state = _states[state_name];
        state.name = state_name;

        const auto &animation_node = node.child("animation");
        if (animation_node)
        {
            const auto &frame_nodes = animation_node.children("frame");
            for (const auto &frame_node : frame_nodes)
            {
                int frame_id = frame_node.attribute("tileid").as_int();
                double frame_duration = frame_node.attribute("duration").as_double() / 1000.0;
                engine::Rectangle source_rect = engine::Rectangle::EMPTY;
                if (_sprite_sheet)
                {
                    source_rect = _sprite_sheet->get_source_rect(frame_id);
                }

                SpriteFrame frame(frame_id, frame_duration, source_rect);
                state.frames.push_back(frame);
            }
        }
    }*/
} // namespace engine::sprite
