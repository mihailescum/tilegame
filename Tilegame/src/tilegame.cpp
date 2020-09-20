#include "tilegame.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <time.h>

#include <sstream>

#include "engine.h"

namespace tilegame
{
    void Tilegame::initialize()
    {
        Game::initialize();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowPos(window.getGLFWwindow(), 700, 400);
    }

    void Tilegame::loadContent()
    {
    }

    void Tilegame::unloadContent()
    {
    }

    void Tilegame::update(const double deltaTime)
    {
        timer += deltaTime;
        updates++;

        // - Reset after one second
        if (timer > 1)
        {
            timer--;
            std::stringstream ss;
            ss << "FPS: " << frames << " - UPS: " << updates;
            window.setTitle(ss.str());
            updates = 0, frames = 0;
        }
    }

    void Tilegame::draw()
    {
        glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        frames++;
    }
} // namespace tilegame
