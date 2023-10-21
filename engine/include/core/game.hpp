#pragma once

#include "graphicsdevice.hpp"
#include "window.hpp"
#include "resourcemanager.hpp"

namespace engine
{
    class Game
    {
    private:
        static const int DEFAULT_WINDOW_WIDTH = 800;
        static const int DEFAULT_WINDOW_HEIGHT = 600;

        bool _shouldRun;

    protected:
        Window window;
        GraphicsDevice _graphics_device;
        ResourceManager _resource_manager;
        double _time_step;

        virtual void initialize();
        virtual void load_content();
        virtual void unload_content();
        virtual void update(double gametime) = 0;
        virtual void begin_update();
        virtual void end_update(){};
        virtual void draw() = 0;
        virtual void begin_draw(){};
        virtual void end_draw(){};
        virtual void resize(int width, int height);

    public:
        Game(int window_width = DEFAULT_WINDOW_WIDTH, int window_height = DEFAULT_WINDOW_HEIGHT) : window(window_width, window_height), _graphics_device(window) {}
        Game(const Game &game) = delete;
        virtual ~Game();

        const Window &get_window() const { return window; }
        const GraphicsDevice &get_graphics_device() const { return _graphics_device; }
        ResourceManager &get_resource_manager() { return _resource_manager; }

        void run();
    };
} // namespace engine
