#pragma once

#include <unordered_map>
#include <string>

#include "tileson/tileson.hpp"

#include "graphics/spritestate.hpp"

namespace engine::graphics
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

        // TODO this should be const, but we get in trouble with Tileson then
        void parse(tson::Tile &data);

        // TODO review if this is necessarys
        void sprite_sheet(SpriteSheet *const sprite_sheet) { _sprite_sheet = sprite_sheet; }

        const SpriteState &operator[](std::string name) const { return _states.at(name); }
        SpriteState &operator[](std::string name) { return _states.at(name); }
    };
} // namespace engine::graphics
