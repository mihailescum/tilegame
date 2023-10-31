#pragma once

#include "game.hpp"

namespace engine
{
    class Entity;
    class Scene
    {
    protected:
        Game &_game;

    public:
        Scene(Game &game);
        Scene(const Scene &scene) = delete;

        Game &get_game() const;

        virtual void initialize(){};
        virtual void load_content(){};
        virtual void unload_content(){};
        virtual void update(const engine::GameTime &update_time){};
        virtual void draw(const engine::GameTime &draw_time){};
    };
}