#ifndef __TILEGAME_TILEGAME_H__
#define __TILEGAME_TILEGAME_H__

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame
{
    class Tilegame : public engine::Game
    {
    public:
        Tilegame() : Game() {}
        Tilegame(const Tilegame &game) = delete;
        ~Tilegame() {}

        engine::SpriteBatch *getSpriteBatch() const;

    protected:
        virtual void initialize() override;
        virtual void loadContent() override;
        virtual void unloadContent() override;
        virtual void processInput() override;
        virtual void update(const double timeStep) override;
        virtual void draw() override;

    private:
        int frames = 0;
        int updates = 0;
        double timer = 0;

        std::unique_ptr<engine::SpriteBatch> spriteBatch;

        std::unique_ptr<worldscene::WorldScene> scene;
    };
} // namespace tilegame

#endif