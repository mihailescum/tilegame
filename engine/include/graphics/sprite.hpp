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

        /** @brief Same as parse(const tson::Tile&), but reads one tile's raw Tiled JSON object
         * ("type", "properties", "animation" fields) instead of tileson's object model - used
         * when maps/tilesets are loaded via Lua (see systems::Script::parse_sprite_animations()),
         * which hands over a tile's raw JSON rather than a parsed tson::Tile. Since there's no
         * backing SpriteSheet/texture in that path, `columns` and `tile_dimensions` (the owning
         * tileset's atlas layout) are passed explicitly instead of being read off
         * `_sprite_sheet`. */
        void parse(const json11::Json &tile_json, int columns, const glm::ivec2 &tile_dimensions);

        // TODO review if this is necessarys
        void sprite_sheet(SpriteSheet *const sprite_sheet) { _sprite_sheet = sprite_sheet; }

        const SpriteState &operator[](std::string name) const { return _states.at(name); }
        SpriteState &operator[](std::string name) { return _states.at(name); }

        /** @brief Whether this sprite defines a state named `name`, without throwing if it doesn't. */
        bool has_state(const std::string &name) const { return _states.count(name) > 0; }
    };
} // namespace engine::graphics
