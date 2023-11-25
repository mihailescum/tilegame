#include "sprite/sprite.hpp"

#include "sprite/spritesheet.hpp"

namespace engine::sprite
{
    void Sprite::parse(tson::Tile &data)
    {
        _name = data.getType();

        if (_sprite_sheet)
        {
            const std::string state_name = data.get<std::string>("state");

            auto &state = _states[state_name];
            state.name = state_name;

            const auto &animation = data.getAnimation();
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
} // namespace engine::sprite
