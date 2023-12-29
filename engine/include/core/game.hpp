#pragma once

// Making sure that GLAD is included before GLFW, because GLAD includes the required headers behind the scenes
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/window.hpp"
#include "core/resourcemanager.hpp"
#include "core/gametime.hpp"
#include "graphics/graphicsdevice.hpp"
#include "graphics/postprocessor.hpp"

namespace engine
{
    class Game
    {
    private:
        static const int DEFAULT_WINDOW_WIDTH = 800;
        static const int DEFAULT_WINDOW_HEIGHT = 600;

        bool _shouldRun;
        GameTime _update_time;
        GameTime _draw_time;

        bool _postprocessing_enabled;

    protected:
        Window _window;
        graphics::GraphicsDevice _graphicsdevice;
        graphics::PostProcessor _postprocessor;
        ResourceManager _resource_manager;
        float _time_step;

        virtual void initialize();
        virtual void load_content();
        virtual void unload_content();
        virtual void update(const engine::GameTime &update_time) = 0;
        virtual void begin_update();
        virtual void end_update(){};
        virtual void draw(const engine::GameTime &draw_time) = 0;
        virtual void begin_draw();
        virtual void end_draw(const engine::GameTime &draw_time);
        virtual void resize(int width, int height);

    public:
        Game(int window_width = DEFAULT_WINDOW_WIDTH, int window_height = DEFAULT_WINDOW_HEIGHT)
            : _window(window_width, window_height),
              _graphicsdevice(_window),
              _postprocessor(_graphicsdevice),
              _postprocessing_enabled(false)
        {
        }

        Game(const Game &game) = delete;
        virtual ~Game();

        const Window &window() const { return _window; }
        const graphics::GraphicsDevice &graphics_device() const { return _graphicsdevice; }

        ResourceManager &resource_manager() { return _resource_manager; }
        const ResourceManager &resource_manager() const { return _resource_manager; }

        void run();

        bool &postprocessing_enabled() { return _postprocessing_enabled; }
    };
} // namespace engine
