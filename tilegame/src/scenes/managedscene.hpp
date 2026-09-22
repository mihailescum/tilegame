#pragma once

#include <any>

#include "scenegraphdata.hpp"

namespace tilegame
{
    class Tilegame;
}

namespace tilegame::scenes
{
    class SceneManager;

    /**
     * @brief Base class for scenes that live on a SceneManager's stack.
     *
     * Adds the ability for a scene to pause its own begin_update()/update()/
     * end_update() and/or begin_draw()/draw()/end_draw() calls without being
     * removed from the stack, and to close itself - popping it from the
     * stack and delivering a result to the callback it was pushed with (if
     * any), returning focus to whichever scene is now on top.
     */
    class ManagedScene : public tilegame::Scene
    {
    private:
        SceneManager &_scene_manager;
        bool _update_paused = false;
        bool _draw_paused = false;

    protected:
        /// Pauses/resumes this scene's begin_update()/update()/end_update() calls.
        void pause_update(bool paused = true) { _update_paused = paused; }
        /// Pauses/resumes this scene's begin_draw()/draw()/end_draw() calls.
        void pause_draw(bool paused = true) { _draw_paused = paused; }

        /// Pops this scene from the SceneManager's stack, delivering `result` to the callback it was pushed with (if any).
        void close(std::any result = {});

    public:
        ManagedScene(Tilegame &game, SceneManager &scene_manager);
        ManagedScene(const ManagedScene &) = delete;
        virtual ~ManagedScene() = default;

        bool update_paused() const { return _update_paused; }
        bool draw_paused() const { return _draw_paused; }
    };
} // namespace tilegame::scenes
