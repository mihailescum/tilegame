#pragma once

#include <utility>
#include "entity/registry.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;

    class InputSystem
    {
    private:
        entt::registry &registry;
        const engine::Window &window;

        void handlePlayerInput(const entt::entity &entity, int key, const std::pair<bool, bool> keyPressed);

    public:
        InputSystem(WorldScene &scene, const engine::Window &window);
        void initialize();
        void processInput();
    };
} // namespace tilegame