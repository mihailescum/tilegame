#pragma once

#include <unordered_map>
#include <string>

#include "pugixml/pugixml.hpp"

#include "sprite/spritestate.hpp"

namespace engine::sprite
{
    class SpriteSheet;

    class Sprite
    {
    private:
        std::string _name;
        SpriteSheet *_sprite_sheet;
        std::unordered_map<std::string, SpriteState> _states;

    public:
        Sprite(std::string name, SpriteSheet *sprite_sheet) : _name(name), _sprite_sheet(sprite_sheet) {}

        void parse(const pugi::xml_node &node, const pugi::xml_node &root_node);
    };
} // namespace engine::sprite
