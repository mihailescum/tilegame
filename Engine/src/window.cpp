#include "window.h"

#include <iostream>

namespace engine
{
    bool Window::isResizable()
    {
        return (this->resizable == GLFW_TRUE) ? true : false;
    }

    void Window::setResizable(const bool resizable)
    {
        this->resizable = (resizable) ? GLFW_TRUE : GLFW_FALSE;
    }

    std::string Window::getTitle()
    {
        return this->title;
    }

    void Window::setTitle(const std::string title)
    {
        if (this->glfwWindow)
        {
            this->title = title;
            glfwSetWindowTitle(this->glfwWindow, this->title.c_str());
        }
    }

    int Window::getWindowWidth()
    {
        return this->window_width;
    }

    int Window::getWindowHeight()
    {
        return this->window_height;
    }

    void Window::setWindowDimensions(const int width, const int height)
    {
        if (this->glfwWindow)
        {
            if (width >= 0)
                this->window_width = width;
            if (height >= 0)
                this->window_width = width;

            glfwSetWindowSize(this->glfwWindow, this->window_width, this->window_height);
        }
    }

    void errorCallback(int error, const char *description)
    {
        std::cerr << "GLFW error " << error << ": " << description << std::endl;
    }

    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    int Window::initialize(const int width, const int height)
    {
        this->window_width = width;
        this->window_height = height;

        int glfwInitRes = glfwInit();
        if (!glfwInitRes)
        {
            std::cerr << "Unable to initialize GLFW" << std::endl;
            return 0;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, this->resizable);

        this->glfwWindow = glfwCreateWindow(window_width, window_height, "InitGL", nullptr, nullptr);
        if (!this->glfwWindow)
        {
            std::cerr << "Unable to create GLFW window" << std::endl;
            glfwTerminate();
            this->glfwWindow = nullptr;
            return 0;
        }

        glfwMakeContextCurrent(this->glfwWindow);

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            std::cerr << "Unable to initialize glad" << std::endl;
            glfwDestroyWindow(this->glfwWindow);
            glfwTerminate();
            this->glfwWindow = nullptr;
            return 0;
        }
        glViewport(0, 0, window_width, window_height);

        glfwSetFramebufferSizeCallback(this->glfwWindow, framebuffer_size_callback);

        return 1;
    }

    Window::~Window()
    {
        if (this->glfwWindow)
            glfwDestroyWindow(this->glfwWindow);
        glfwTerminate();
    }

    bool Window::shouldClose()
    {
        if (!glfwWindow)
            return true;
        else
            return glfwWindowShouldClose(glfwWindow);
    }

    GLFWwindow *Window::getGLFWwindow()
    {
        return this->glfwWindow;
    }
} // namespace engine