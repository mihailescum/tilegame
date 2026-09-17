#include "graphics/sprite.hpp"

#include "graphics/spritesheet.hpp"

namespace engine::graphics
{
    void Sprite::parse(const tson::Tile &data)
    {
        _name = data.getClassType();

        if (_sprite_sheet)
        {
            // 'const_cast' is okay, because tson::Tile::get<> should have been declared 'const'
            const std::string state_name = const_cast<tson::Tile &>(data).get<std::string>(Sprite::NAME_SPRITE_STATE);

            auto &state = _states[state_name];
            state.name = state_name;

            // 'const_cast' is okay, because tson::Tile::getAnimation should have been declared 'const'
            const auto &animation = const_cast<tson::Tile &>(data).getAnimation();
            if (animation.size() > 0)
            {
                const auto &tson_frames = animation.getFrames();
                for (const auto &tson_frame : tson_frames)
                {
                    int frame_id = tson_frame.getTileId() - 1; // Tileson starts the IDs at 1, but we start at 0.
                    float frame_duration = tson_frame.getDuration() / 1000.0;

                    const auto &source_rect = _sprite_sheet->source_rect(frame_id);

                    SpriteFrame frame(frame_id, frame_duration, source_rect);
                    state.frames.push_back(frame);
                }
            }
        }
    }

    void Sprite::parse(const json11::Json &tile_json, int columns, const glm::ivec2 &tile_dimensions)
    {
        _name = tile_json["type"].string_value();

        std::string state_name;
        for (const auto &property : tile_json["properties"].array_items())
        {
            if (property["name"].string_value() == Sprite::NAME_SPRITE_STATE)
            {
                state_name = property["value"].string_value();
            }
        }

        auto &state = _states[state_name];
        state.name = state_name;

        for (const auto &frame_json : tile_json["animation"].array_items())
        {
            int frame_id = frame_json["tileid"].int_value();
            float frame_duration = static_cast<float>(frame_json["duration"].number_value()) / 1000.0f;

            int column = frame_id % columns;
            int row = frame_id / columns;
            engine::Rectangle source_rect(glm::vec2(column * tile_dimensions.x, row * tile_dimensions.x), tile_dimensions);

            SpriteFrame frame(frame_id, frame_duration, source_rect);
            state.frames.push_back(frame);
        }
    }
} // namespace engine::graphics
