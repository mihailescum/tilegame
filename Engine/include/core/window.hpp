#pragma once

#include <string>

#include <GLFW/glfw3.h>

namespace engine
{
    class Window
    {
    private:
        GLFWwindow *glfwWindow;

        std::string title;
        int windowWidth;
        int windowHeight;
        int resizable = GLFW_FALSE;

    public:
        Window(const int width, const int height);
        virtual ~Window();

        /*
            Creates and show a window using the given dimensions
        */
        int initialize();
        /*
            Indicates that the window should close at the next possible point in time 
        */
        bool shouldClose() const;
        void setPosition(const int x, const int y);
        bool isKeyPressed(int keyCode) const;

        GLFWwindow *getGLFWwindow() const;
        bool isResizable();
        void setResizable(const bool resizable);
        std::string getTitle() const;
        void setTitle(const std::string &title);
        int getWindowWidth() const;
        int getWindowHeight() const;
        /*
            Set the dimensions of the window. Passing negative values will leave the properties unchanged
        */
        void setWindowDimensions(const int width, const int height);

        //__event void windowSizeChanged(int width, int height);
    };
} // namespace engine
