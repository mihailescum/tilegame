#pragma once

#include <utility>
#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class InputSystem
    {
    private:
        entt::registry &registry;
        const engine::Window &window;

        void handlePlayerInput(const entt::entity &entity, int key, const std::pair<bool, bool> keyPressed);

    public:
        InputSystem(engine::Scene &scene, const engine::Window &window) : registry(scene.getRegistry()), window(window) {}
        void initialize();
        void processInput();
    };
} // namespace tilegame