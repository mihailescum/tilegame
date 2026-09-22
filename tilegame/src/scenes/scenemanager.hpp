#pragma once

#include <any>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "engine.hpp"

#include "scenes/managedscene.hpp"

namespace tilegame
{
    class Tilegame;
}

namespace tilegame::scenes
{
    /// Whether a newly pushed scene replaces the current top of the stack or is added above it.
    enum class SceneTransition
    {
        Push,
        Replace
    };

    /**
     * @brief Owns the stack of active ManagedScenes and drives their lifecycle each frame.
     *
     * Scenes are updated/drawn bottom-to-top, skipping any scene that has
     * paused its own update or draw (see ManagedScene::pause_update()/
     * pause_draw()). Pushing, replacing and closing scenes only queue the
     * change: it is applied at the start of the next update/draw pass so the
     * stack is never mutated while it is being iterated, which lets a scene
     * safely push a new scene or close itself from within its own update()
     * or draw().
     */
    class SceneManager
    {
    public:
        using CloseCallback = std::function<void(std::any)>;

        explicit SceneManager(Tilegame &game) : _game(game) {}
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
        void close(ManagedScene &scene, std::any result = {});

        void update(const engine::GameTime &update_time);
        void begin_update();
        void end_update();
        void draw(const engine::GameTime &draw_time);
        void begin_draw();
        void end_draw(const engine::GameTime &draw_time);

        /// Unloads and clears every scene still on the stack. Called once by Tilegame during shutdown.
        void unload_content();

    private:
        struct Entry
        {
            std::unique_ptr<ManagedScene> scene;
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
            std::unique_ptr<ManagedScene> scene;
            CloseCallback on_close;
            SceneTransition transition = SceneTransition::Push;
            ManagedScene *target = nullptr;
            std::any result;
        };

        Tilegame &_game;
        std::vector<Entry> _stack;
        std::vector<PendingAction> _pending;

        template <typename SceneT, typename... Args>
        SceneT &push_scene(SceneTransition transition, CloseCallback on_close, Args &&...args)
        {
            static_assert(std::is_base_of_v<ManagedScene, SceneT>, "SceneT must derive from tilegame::scenes::ManagedScene");

            auto scene = std::make_unique<SceneT>(_game, *this, std::forward<Args>(args)...);
            scene->initialize();
            scene->load_content();
            SceneT &scene_ref = *scene;

            PendingAction action;
            action.kind = PendingKind::Push;
            action.scene = std::move(scene);
            action.on_close = std::move(on_close);
            action.transition = transition;
            _pending.push_back(std::move(action));

            return scene_ref;
        }

        void flush_pending();
    };
} // namespace tilegame::scenes
