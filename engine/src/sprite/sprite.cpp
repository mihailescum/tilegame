#include "sprite/sprite.hpp"

#include <sstream>

namespace engine::sprite
{
    void Sprite::parse(const pugi::xml_node &node)
    {
        auto state_nodes = node.children("wangcolor");
        std::vector<SpriteState> sprite_states;
        for (auto state_node : state_nodes)
        {
            std::string name = state_node.attribute("name").as_string();
            SpriteState state(name);
            sprite_states.push_back(state);
        }

        auto frame_nodes = node.children("wangtile");
        for (auto frame_node : frame_nodes)
        {
            int id = frame_node.attribute("tileid").as_int();
            SpriteFrame frame(id);

            int state_id = 0;
            std::stringstream ss(frame_node.attribute("wangid").as_string());
            for (int i; ss >> i;)
            {
                if (i > 0)
                {
                    state_id = i;
                    break;
                }
                if (ss.peek() == ',')
                {
                    ss.ignore();
                }
            }
            sprite_states[state_id - 1].frames.push_back(frame);
        }

        for (auto state : sprite_states)
        {
            _states[state.name] = state;
        }
    }
} // namespace engine::sprite