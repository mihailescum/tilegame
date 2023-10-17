#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "tilegame.hpp"
#include <fstream>
#include <sstream>

int main(int argc, char *argv[])
{
    tilegame::Tilegame game;
    game.run();

    return 0;
}