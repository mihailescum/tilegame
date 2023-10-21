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
        double _timer = 0;

        engine::SpriteBatch _spritebatch;
        worldscene::WorldScene _worldscene;

    protected:
        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(double gametime) override;
        virtual void draw() override;

    public:
        Tilegame() : Game(), _spritebatch(_graphics_device), _worldscene(*this) {}
        Tilegame(const Tilegame &game) = delete;
        ~Tilegame() {}

        engine::SpriteBatch &get_spritebatch() { return _spritebatch; }
    };
} // namespace tilegame