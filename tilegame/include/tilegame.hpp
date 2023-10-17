#ifndef __TILEGAME_TILEGAME_H__
#define __TILEGAME_TILEGAME_H__

#include "engine.hpp"

// #include "worldscene/worldscene.hpp"

namespace tilegame
{
    class Tilegame : public engine::Game
    {
    private:
        int frames = 0;
        int updates = 0;
        double timer = 0;

        engine::SpriteBatch spriteBatch;

    protected:
        virtual void initialize() override;
        virtual void loadContent() override;
        virtual void unloadContent() override;
        virtual void processInput() override;
        virtual void update(const double timeStep) override;
        virtual void draw() override;

    public:
        Tilegame() : Game(), spriteBatch(graphicsDevice) {}
        Tilegame(const Tilegame &game) = delete;
        ~Tilegame() {}

        engine::SpriteBatch &getSpriteBatch() { return spriteBatch; }
    };
} // namespace tilegame

#endif