#pragma once

#include "game.hpp"
#include "scenegraphnode.hpp"

namespace engine
{
    class Entity;

    template <typename T, typename = std::is_default_constructible<T>>
    class Scene
    {
    protected:
        Game &_game;
        SceneGraphNode<T> _scene_graph_root;

    public:
        Scene(Game &game) : _game(game) {}
        Scene(const Scene &scene) = delete;

        Game &get_game() const { return _game; }
        SceneGraphNode<T> &get_scene_graph_root() { return _scene_graph_root; }

        virtual void initialize(){};
        virtual void load_content(){};
        virtual void unload_content(){};
        virtual void update(const engine::GameTime &update_time){};
        virtual void draw(const engine::GameTime &draw_time){};
    };
}