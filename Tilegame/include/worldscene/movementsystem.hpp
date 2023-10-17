#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;

    class MovementSystem
    {
    private:
        struct Impl;
        std::unique_ptr<MovementSystem::Impl> impl;
        
        template <typename T>
        static int sgn(T val) { return (T(0) < val) - (val < T(0)); }

    public:
        MovementSystem(WorldScene &scene);
        ~MovementSystem();

        void initialize();
        void update(const double deltaTime);

        void setPosition(const entt::entity &entity, const double x, const double y);
    };
} // namespace tilegame::worldscene