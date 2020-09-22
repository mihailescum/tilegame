#ifndef __ENGINE_WINDOW_H__
#define __ENGINE_WINDOW_H__

#include <string>

#include <GLFW/glfw3.h>

namespace engine
{
    class Window
    {
    public:
        Window(const int width, const int height);
        ~Window();
        bool isResizable();
        void setResizable(const bool resizable);
        std::string getTitle() const;
        void setTitle(const std::string title);
        int getWindowWidth() const;
        int getWindowHeight() const;
        void setPosition(const int x, const int y);
        /*
            Creates and show a window using the given dimensions
        */
        int initialize();

        /*
            Set the dimensions of the window. Passing negative values will leave the properties unchanged
        */
        void setWindowDimensions(const int width, const int height);
        GLFWwindow *getGLFWwindow() const;

        /*
            Indicates that the window should close at the next possible point in time 
        */
        bool shouldClose() const;

        //__event void windowSizeChanged(int width, int height);

    private:
        std::string title;
        int windowWidth;
        int windowHeight;
        int resizable = GLFW_FALSE;

        GLFWwindow *glfwWindow;
    };
} // namespace engine

#endif