#pragma once
#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame
{
    class Tilegame : public engine::Game
    {
    private:
        int frames = 0;
        int updates = 0;
        double timer = 0;

        engine::SpriteBatch spriteBatch;
        worldscene::WorldScene worldScene;

    protected:
        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(const double _time_step) override;
        virtual void draw() override;

    public:
        Tilegame() : Game(), spriteBatch(_graphics_device), worldScene(*this) {}
        Tilegame(const Tilegame &game) = delete;
        ~Tilegame() {}

        engine::SpriteBatch &getSpriteBatch() { return spriteBatch; }
    };
} // namespace tilegame