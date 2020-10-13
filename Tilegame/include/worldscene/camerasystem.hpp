#pragma once

#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class CameraSystem
    {
    private:
        std::unique_ptr<engine::Observer> cameraObserver;
        entt::registry &registry;

    public:
        CameraSystem(engine::Scene &scene) : registry(scene.getRegistry()) {}
        void initialize();
        void update(const double deltaTime);
    };
} // namespace tilegame::worldscene