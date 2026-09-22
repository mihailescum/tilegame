#pragma once

#include <any>

#include "core/gametime.hpp"

namespace engine
{
    class Game;
    class SceneManager;

    /**
     * @brief Base class for a game screen/level, mirroring Game's lifecycle hooks (initialize/update/draw/...).
     *
     * Concrete scenes derive from Scene and override the hooks they need;
     * most are optional no-ops except initialize(). Lives on a SceneManager's
     * stack, which is what actually drives these lifecycle hooks each frame
     * (see SceneManager::update()/draw()) rather than some owning Game
     * calling them directly. A scene can pause its own update and/or draw
     * without leaving the stack (pause_update()/pause_draw()), push further
     * scenes on top of itself (scene_manager()), and close itself - popping
     * it from the stack and delivering a result to the callback it was
     * pushed with, if any (close()) - returning focus to whichever scene is
     * now on top. Reaches its SceneManager through the owning Game's
     * scene_manager(), the same way it reaches resource_manager() - not
     * through its own constructor - so a scene never needs to take one.
     */
    class Scene
    {
    protected:
        Game &_game;

    private:
        bool _update_paused = false;
        bool _draw_paused = false;

    protected:
        /// Pops this scene from the SceneManager's stack, delivering `result` to the callback it was pushed with (if any). Defined in scene.cpp, once Game/SceneManager are complete.
        void close(std::any result = {});

        /// The SceneManager this scene lives on - lets a scene push further scenes on top of itself (e.g. a UI overlay). Defined in scene.cpp, once Game is complete.
        SceneManager &scene_manager();

    public:
        Scene(Game &game) : _game(game) {}
        Scene(const Scene &scene) = delete;
        virtual ~Scene() {}

        const Game &game() const { return _game; }
        Game &game() { return _game; }

        bool update_paused() const { return _update_paused; }
        bool draw_paused() const { return _draw_paused; }
        /// Pauses/resumes this scene's begin_update()/update()/end_update() calls.
        void pause_update(bool paused = true) { _update_paused = paused; }
        /// Pauses/resumes this scene's begin_draw()/draw()/end_draw() calls.
        void pause_draw(bool paused = true) { _draw_paused = paused; }

        virtual void initialize() = 0;
        virtual void load_content() {};
        virtual void unload_content() {};
        virtual void update(const engine::GameTime &update_time) {};
        virtual void begin_update() {};
        virtual void end_update() {};
        virtual void draw(const engine::GameTime &draw_time) {};
        virtual void begin_draw() {};
        virtual void end_draw(const engine::GameTime &draw_time) {};
    };
}
