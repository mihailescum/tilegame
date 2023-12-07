#pragma once
#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame
{
    class Tilegame : public engine::Game
    {
    private:
        int _frames = 0;
        int _updates = 0;
        float _timer = 0;

        engine::graphics::SpriteBatch _spritebatch;
        worldscene::WorldScene _worldscene;

    protected:
        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(const engine::GameTime &update_time) override;
        virtual void draw(const engine::GameTime &draw_time) override;

    public:
        Tilegame(int window_width, int window_height) : Game(window_width, window_height), _spritebatch(_graphics_device), _worldscene(*this) {}
        Tilegame(const Tilegame &game) = delete;
        ~Tilegame() {}

        engine::graphics::SpriteBatch &spritebatch() { return _spritebatch; }
    };
} // namespace tilegame