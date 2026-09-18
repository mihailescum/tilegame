#pragma once

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>

#include "tileson/tileson.hpp"

#include "graphics/spritestate.hpp"

namespace engine::graphics
{
    class SpriteSheet;

    /**
     * @brief A named, animated entity within a SpriteSheet, made up of one or more
     * named SpriteStates (e.g. "walk_down", "idle_up"), each holding its own frame
     * sequence. Parsed from the custom "state" tile property and animation data of
     * a Tiled tileset tile.
     */
    class Sprite
    {
    private:
        inline static const std::string NAME_SPRITE_STATE = "state";

        std::string _name;
        SpriteSheet *_sprite_sheet;
        std::unordered_map<std::string, SpriteState> _states;

    public:
        Sprite() : Sprite("", nullptr) {}
        Sprite(std::string name, SpriteSheet *sprite_sheet) : _name(name), _sprite_sheet(sprite_sheet) {}

        /** @brief Parses one Tiled tile's animation into the SpriteState named by its "state" custom property, appending a SpriteFrame per animation frame. */
        void parse(const tson::Tile &data);

        // TODO review if this is necessarys
        void sprite_sheet(SpriteSheet *const sprite_sheet) { _sprite_sheet = sprite_sheet; }

        const SpriteState &operator[](std::string name) const { return _states.at(name); }
        SpriteState &operator[](std::string name) { return _states.at(name); }

        /** @brief Whether this sprite defines a state named `name`, without throwing if it doesn't. */
        bool has_state(const std::string &name) const { return _states.count(name) > 0; }
    };
} // namespace engine::graphics
