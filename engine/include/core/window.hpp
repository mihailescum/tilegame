#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace engine
{
    class Window
    {
    private:
        GLFWwindow *_native_window;

        std::string _title;
        int _width;
        int _height;
        bool _resizable = GLFW_FALSE;

    public:
        Window(int width, int height);
        virtual ~Window();

        /*
            Creates and show a window using the given dimensions
        */
        int initialize();
        /*
            Indicates that the window should close at the next possible point in time
        */
        bool should_close() const;
        void position(int x, int y);
        bool is_key_pressed(int key_code) const;

        const GLFWwindow &native_window() const { return *_native_window; }
        GLFWwindow &native_window() { return *_native_window; }
        bool resizable() const;
        void resizable(bool resizable);
        std::string title() const;
        void title(const std::string &title);
        int width() const { return _width; }
        int height() const { return _height; }
        /*
            Set the dimensions of the window. Passing negative values will leave the properties unchanged
        */
        void window_dimensions(int width, int height);

        //__event void windowSizeChanged(int width, int height);
    };
} // namespace engine
