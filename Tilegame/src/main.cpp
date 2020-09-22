#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "tilegame.hpp"
#include <fstream>
#include <sstream>

void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

int main(int argc, char *argv[])
{
    glfwSetErrorCallback(errorCallback);
    tilegame::Tilegame game = tilegame::Tilegame();
    game.run();

    return 0;
}