## Installation

To install you need the following prerequisites:

* C++17 compiler (e.g. g++8.0 as it requieres `std::filesystem` capabilities)
* Lua 5.2+ installed
* [GLFW](https://www.glfw.org/) -- After installing its prerequisites, run `cmake` to create makefile, afterwards build it with `make` and install it with `make install`)
* [glm](https://glm.g-truc.net/) -- Download the source, afterwards copy the headers (the whole `glm` directory) to a location, where system wide include files can be found. On Ubuntu copy the `glm` directory to `/usr/local/include` and run `sudo chmod -R 755 /usr/local/include/glm` for correct permissions
* [entt](https://github.com/skypjack/entt/)


Afterwards, you can run (from a separate `build` folder inside the root folder of this project) `cmake ..` and build everything with `make`.