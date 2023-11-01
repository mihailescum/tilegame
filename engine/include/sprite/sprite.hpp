#pragma once

#include <unordered_map>
#include <string>

#include "pugixml/pugixml.hpp"

#include "sprite/spritestate.hpp"

namespace engine::sprite
{
    class Sprite
    {
    private:
        std::string _name;
        std::unordered_map<std::string, SpriteState> _states;

    public:
        Sprite(std::string name) : _name(name) {}

        void parse(const pugi::xml_node &node);
    };
} // namespace engine::sprite
