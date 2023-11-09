#pragma once

#include <unordered_map>
#include <string>

#include "tileson/tileson.hpp"

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
        Sprite() : Sprite("", nullptr) {}
        Sprite(std::string name, SpriteSheet *sprite_sheet) : _name(name), _sprite_sheet(sprite_sheet) {}

        void parse(const tson::Tile &data);
        void set_sprite_sheet(SpriteSheet *const sprite_sheet) { _sprite_sheet = sprite_sheet; }

        const SpriteState &operator[](std::string name) const { return _states.at(name); }
        SpriteState &operator[](std::string name) { return _states.at(name); }
    };
} // namespace engine::sprite
