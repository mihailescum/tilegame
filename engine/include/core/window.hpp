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
        int width;
        int height;
        bool resizable = GLFW_FALSE;

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
        bool shouldClose() const;
        void setPosition(int x, int y);
        bool isKeyPressed(int keyCode) const;

        GLFWwindow *getGLFWwindow() const;
        bool isResizable();
        void setResizable(const bool resizable);
        std::string getTitle() const;
        void setTitle(const std::string &title);
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        /*
            Set the dimensions of the window. Passing negative values will leave the properties unchanged
        */
        void setWindowDimensions(int width, int height);

        //__event void windowSizeChanged(int width, int height);
    };
} // namespace engine
