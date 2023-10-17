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
        unsigned int width;
        unsigned int height;
        bool resizable = GLFW_FALSE;

    public:
        Window(unsigned int width, unsigned int height);
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
        unsigned int getWidth() const { return width; }
        unsigned int getHeight() const { return height; }
        /*
            Set the dimensions of the window. Passing negative values will leave the properties unchanged
        */
        void setWindowDimensions(unsigned int width, unsigned int height);

        //__event void windowSizeChanged(int width, int height);
    };
} // namespace engine
