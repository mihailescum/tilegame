#include "core/window.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine
{
    Window::Window(const int width, const int height) : windowWidth(width), windowHeight(height)
    {
    }

    bool Window::isResizable()
    {
        return (this->resizable == GLFW_TRUE) ? true : false;
    }

    void Window::setResizable(const bool resizable)
    {
        this->resizable = (resizable) ? GLFW_TRUE : GLFW_FALSE;
    }

    std::string Window::getTitle() const
    {
        return this->title;
    }

    void Window::setTitle(const std::string &title)
    {
        if (this->glfwWindow)
        {
            this->title = title;
            glfwSetWindowTitle(this->glfwWindow, this->title.c_str());
        }
    }

    int Window::getWindowWidth() const
    {
        return this->windowWidth;
    }

    int Window::getWindowHeight() const
    {
        return this->windowHeight;
    }

    void Window::setWindowDimensions(const int width, const int height)
    {
        if (this->glfwWindow)
        {
            if (width >= 0)
                this->windowWidth = width;
            if (height >= 0)
                this->windowHeight = height;

            glfwSetWindowSize(this->glfwWindow, this->windowWidth, this->windowHeight);
        }
    }

    void Window::setPosition(const int x, const int y)
    {
        glfwSetWindowPos(this->glfwWindow, 700, 400);
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
        glfwWindowHint(GLFW_RESIZABLE, this->resizable);

        this->glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "InitGL", nullptr, nullptr);
        if (!this->glfwWindow)
        {
            Log::e("Unable to create GLFW window");

            glfwTerminate();
            this->glfwWindow = nullptr;
            return 0;
        }

        glfwMakeContextCurrent(this->glfwWindow);

        glfwSetFramebufferSizeCallback(this->glfwWindow, framebuffer_size_callback);

        return 1;
    }

    Window::~Window()
    {
        if (this->glfwWindow)
            glfwDestroyWindow(this->glfwWindow);
        glfwTerminate();
        this->glfwWindow = nullptr;
    }

    bool Window::shouldClose() const
    {
        if (!glfwWindow)
            return true;
        else
            return glfwWindowShouldClose(this->glfwWindow);
    }

    bool Window::isKeyPressed(int keyCode) const
    {
        return glfwGetKey(this->glfwWindow, keyCode);
    }

    GLFWwindow *Window::getGLFWwindow() const
    {
        return this->glfwWindow;
    }
} // namespace engine