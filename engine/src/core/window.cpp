#include "core/window.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine
{
    Window::Window(int width, int height) : _width(width), _height(height)
    {
    }

    bool Window::resizable() const
    {
        return (this->_resizable == GLFW_TRUE) ? true : false;
    }

    void Window::resizable(const bool resizable)
    {
        this->_resizable = (resizable) ? GLFW_TRUE : GLFW_FALSE;
    }

    std::string Window::title() const
    {
        return this->_title;
    }

    void Window::title(const std::string &title)
    {
        if (this->_native_window)
        {
            this->_title = title;
            glfwSetWindowTitle(this->_native_window, this->_title.c_str());
        }
    }

    void Window::window_dimensions(int width, int height)
    {
        if (this->_native_window)
        {
            if (width >= 0)
                this->_width = width;
            if (height >= 0)
                this->_height = height;

            glfwSetWindowSize(this->_native_window, this->_width, this->_height);
        }
    }

    void Window::position(const int x, const int y)
    {
        glfwSetWindowPos(this->_native_window, 700, 400);
    }

    void errorCallback(int error, const char *description)
    {
        Log::e("GLFW error ", error, ": ", description);
    }

    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // __raise windowSizeChanged(width, height);
    }

    int Window::initialize()
    {
        glfwSetErrorCallback(errorCallback);
        int glfwInitRes = glfwInit();
        if (!glfwInitRes)
        {
            Log::e("Unable to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, this->_resizable);

        this->_native_window = glfwCreateWindow(_width, _height, "InitGL", nullptr, nullptr);
        if (!this->_native_window)
        {
            Log::e("Unable to create GLFW window");

            glfwTerminate();
            this->_native_window = nullptr;
            return 0;
        }

        glfwMakeContextCurrent(this->_native_window);

        glfwSetFramebufferSizeCallback(this->_native_window, framebuffer_size_callback);

        return 1;
    }

    Window::~Window()
    {
        if (this->_native_window)
            glfwDestroyWindow(this->_native_window);
        glfwTerminate();
        this->_native_window = nullptr;
    }

    bool Window::should_close() const
    {
        if (!_native_window)
            return true;
        else
            return glfwWindowShouldClose(this->_native_window);
    }

    bool Window::is_key_pressed(int keyCode) const
    {
        return glfwGetKey(this->_native_window, keyCode);
    }
} // namespace engine