#include "scenes/scenemanager.hpp"

#include <algorithm>

namespace tilegame::scenes
{
    void SceneManager::close(ManagedScene &scene, std::any result)
    {
        PendingAction action;
        action.kind = PendingKind::Close;
        action.target = &scene;
        action.result = std::move(result);
        _pending.push_back(std::move(action));
    }

    void SceneManager::flush_pending()
    {
        if (_pending.empty())
            return;

        std::vector<PendingAction> pending = std::move(_pending);
        _pending.clear();

        for (auto &action : pending)
        {
            if (action.kind == PendingKind::Push)
            {
                if (action.transition == SceneTransition::Replace && !_stack.empty())
                {
                    _stack.back().scene->unload_content();
                    _stack.pop_back();
                }

                _stack.push_back(Entry{std::move(action.scene), std::move(action.on_close)});
            }
            else // PendingKind::Close
            {
                auto it = std::find_if(_stack.begin(), _stack.end(), [&](const Entry &entry)
                                        { return entry.scene.get() == action.target; });

                if (it != _stack.end())
                {
                    CloseCallback on_close = std::move(it->on_close);
                    it->scene->unload_content();
                    _stack.erase(it);

                    if (on_close)
                        on_close(std::move(action.result));
                }
            }
        }
    }

    void SceneManager::begin_update()
    {
        flush_pending();

        for (auto &entry : _stack)
            if (!entry.scene->update_paused())
                entry.scene->begin_update();
    }

    void SceneManager::update(const engine::GameTime &update_time)
    {
        for (auto &entry : _stack)
            if (!entry.scene->update_paused())
                entry.scene->update(update_time);
    }

    void SceneManager::end_update()
    {
        for (auto &entry : _stack)
            if (!entry.scene->update_paused())
                entry.scene->end_update();
    }

    void SceneManager::begin_draw()
    {
        flush_pending();

        for (auto &entry : _stack)
            if (!entry.scene->draw_paused())
                entry.scene->begin_draw();
    }

    void SceneManager::draw(const engine::GameTime &draw_time)
    {
        for (auto &entry : _stack)
            if (!entry.scene->draw_paused())
                entry.scene->draw(draw_time);
    }

    void SceneManager::end_draw(const engine::GameTime &draw_time)
    {
        for (auto &entry : _stack)
            if (!entry.scene->draw_paused())
                entry.scene->end_draw(draw_time);
    }

    void SceneManager::unload_content()
    {
        flush_pending();

        for (auto &entry : _stack)
            entry.scene->unload_content();

        _stack.clear();
        _pending.clear();
    }
} // namespace tilegame::scenes
