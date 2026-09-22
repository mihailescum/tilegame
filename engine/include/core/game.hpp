#pragma once

// Making sure that GLAD is included before GLFW, because GLAD includes the required headers behind the scenes
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "core/window.hpp"
#include "core/resourcemanager.hpp"
#include "core/scenemanager.hpp"
#include "core/gametime.hpp"
#include "graphics/graphicsdevice.hpp"
#include "graphics/postprocessor.hpp"

namespace engine
{
    /**
     * @brief Base class for the game application: owns the window, graphics device, resource manager and scene manager, and drives the fixed-timestep update/variable-rate draw loop.
     *
     * Concrete games derive from Game and override the lifecycle hooks
     * (initialize/load_content/update/draw/...) rather than reimplementing run(). A Scene reaches
     * the SceneManager it lives on through its owning Game's scene_manager() - the same way it
     * reaches resource_manager() - rather than needing it passed to its constructor.
     */
    class Game
    {
    private:
        static const int DEFAULT_WINDOW_WIDTH = 800;
        static const int DEFAULT_WINDOW_HEIGHT = 600;

        bool _shouldRun;
        GameTime _update_time;
        GameTime _draw_time;

    protected:
        Window _window;
        graphics::GraphicsDevice _graphicsdevice;
        ResourceManager _resource_manager;
        SceneManager _scene_manager;
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
              _scene_manager(*this)
        {
        }

        Game(const Game &game) = delete;
        virtual ~Game();

        const Window &window() const { return _window; }
        const graphics::GraphicsDevice &graphicsdevice() const { return _graphicsdevice; }
        graphics::GraphicsDevice &graphicsdevice() { return _graphicsdevice; }

        ResourceManager &resource_manager() { return _resource_manager; }
        const ResourceManager &resource_manager() const { return _resource_manager; }

        SceneManager &scene_manager() { return _scene_manager; }
        const SceneManager &scene_manager() const { return _scene_manager; }

        /// Initializes the window/graphics device and runs the main loop (fixed-timestep update, uncapped draw) until the window is closed.
        void run();
    };
} // namespace engine
