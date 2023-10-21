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
        void set_position(int x, int y);
        bool is_key_pressed(int key_code) const;

        GLFWwindow *get_native_window() const;
        bool is_resizable();
        void set_resizable(const bool resizable);
        std::string get_title() const;
        void set_title(const std::string &title);
        int get_width() const { return _width; }
        int get_height() const { return _height; }
        /*
            Set the dimensions of the window. Passing negative values will leave the properties unchanged
        */
        void set_window_dimensions(int width, int height);

        //__event void windowSizeChanged(int width, int height);
    };
} // namespace engine
