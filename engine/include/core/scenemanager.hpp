#pragma once

#include <any>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "scene.hpp"

namespace engine
{
    /// Whether a newly pushed scene replaces the current top of the stack or is added above it.
    enum class SceneTransition
    {
        Push,
        Replace
    };

    /**
     * @brief Owns the stack of active Scenes and drives their lifecycle each frame.
     *
     * Scenes are updated/drawn bottom-to-top, skipping any scene that has paused its own update
     * or draw (see Scene::pause_update()/pause_draw()). Pushing, replacing and closing scenes
     * only queue the change: it is applied at the start of the next update/draw pass so the
     * stack is never mutated while it is being iterated, which lets a scene safely push a new
     * scene or close itself from within its own update() or draw().
     */
    class SceneManager
    {
    public:
        using CloseCallback = std::function<void(std::any)>;

        explicit SceneManager(Game &game) : _game(game) {}
        SceneManager(const SceneManager &) = delete;
        ~SceneManager() = default;

        /// Pushes a new scene on top of the stack. Its initialize()/load_content() run immediately.
        template <typename SceneT, typename... Args>
        SceneT &push(Args &&...args)
        {
            return push_scene<SceneT>(SceneTransition::Push, nullptr, std::forward<Args>(args)...);
        }

        /// Like push(), but on_close is invoked with the scene's close() result once it closes itself.
        template <typename SceneT, typename... Args>
        SceneT &push_for_result(CloseCallback on_close, Args &&...args)
        {
            return push_scene<SceneT>(SceneTransition::Push, std::move(on_close), std::forward<Args>(args)...);
        }

        /// Pops the current top of the stack (if any) and pushes a new scene in its place.
        template <typename SceneT, typename... Args>
        SceneT &replace(Args &&...args)
        {
            return push_scene<SceneT>(SceneTransition::Replace, nullptr, std::forward<Args>(args)...);
        }

        /// Like replace(), but on_close is invoked with the scene's close() result once it closes itself.
        template <typename SceneT, typename... Args>
        SceneT &replace_for_result(CloseCallback on_close, Args &&...args)
        {
            return push_scene<SceneT>(SceneTransition::Replace, std::move(on_close), std::forward<Args>(args)...);
        }

        /// Requests that `scene` be popped from the stack, delivering `result` to the callback it was pushed with (if any).
        void close(Scene &scene, std::any result = {});

        void update(const GameTime &update_time);
        void begin_update();
        void end_update();
        void draw(const GameTime &draw_time);
        void begin_draw();
        void end_draw(const GameTime &draw_time);

        /// Unloads and clears every scene still on the stack. Called once by the owning Game during shutdown.
        void unload_content();

    private:
        struct Entry
        {
            std::unique_ptr<Scene> scene;
            CloseCallback on_close;
        };

        enum class PendingKind
        {
            Push,
            Close
        };

        struct PendingAction
        {
            PendingKind kind;
            std::unique_ptr<Scene> scene;
            CloseCallback on_close;
            SceneTransition transition = SceneTransition::Push;
            Scene *target = nullptr;
            std::any result;
        };

        Game &_game;
        std::vector<Entry> _stack;
        std::vector<PendingAction> _pending;

        template <typename SceneT, typename... Args>
        SceneT &push_scene(SceneTransition transition, CloseCallback on_close, Args &&...args)
        {
            static_assert(std::is_base_of_v<Scene, SceneT>, "SceneT must derive from engine::Scene");

            auto scene = std::make_unique<SceneT>(_game, std::forward<Args>(args)...);
            SceneT &scene_ref = *scene;

            // Queued before initialize()/load_content() run, not after: either can synchronously
            // trigger a nested push_scene() (e.g. a script run from load_content() calling
            // _show_message, which pushes a UI scene) whose own pending action must land *after*
            // this one - otherwise it would end up lower in the stack than the scene that's
            // still in the middle of being pushed, and get drawn first instead of on top of it.
            PendingAction action;
            action.kind = PendingKind::Push;
            action.scene = std::move(scene);
            action.on_close = std::move(on_close);
            action.transition = transition;
            _pending.push_back(std::move(action));

            scene_ref.initialize();
            scene_ref.load_content();

            return scene_ref;
        }

        void flush_pending();
    };
} // namespace engine
