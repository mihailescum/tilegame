#pragma once
#include "engine.hpp"

namespace tilegame
{
    /**
     * @brief Top-level engine::Game implementation for the tilegame executable.
     *
     * Pushes WorldScene as the startup scene onto the (engine::Game-owned) SceneManager and
     * forwards the engine's game loop lifecycle callbacks to it, and updates the window title
     * with the current FPS/UPS once per second.
     */
    class Tilegame : public engine::Game
    {
    private:
        int _frames = 0;
        int _updates = 0;
        float _timer = 0;

    protected:
        virtual void initialize() override;
        virtual void load_content() override;
        virtual void update(const engine::GameTime &update_time) override;
        virtual void begin_update() override;
        virtual void end_update() override;
        virtual void draw(const engine::GameTime &draw_time) override;
        virtual void begin_draw() override;
        virtual void end_draw(const engine::GameTime &draw_time) override;

    public:
        Tilegame(int window_width, int window_height) : Game(window_width, window_height) {}
        Tilegame(const Tilegame &game) = delete;
        ~Tilegame() {}
    };
} // namespace tilegame
