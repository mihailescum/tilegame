#pragma once

#include <string>
#include <unordered_map>

#include "engine.hpp"
#include "systems/rendersystem.hpp"

namespace tilegame
{
    class Tilegame;
}

namespace tilegame::worldscene
{
    class WorldScene : public engine::Scene
    {
    private:
        entt::registry _registry;
        tilegame::systems::RenderSystem _rendersystem;

    public:
        WorldScene(Tilegame &game);
        ~WorldScene() = default;

        virtual void initialize() override;
        virtual void load_content() override;
        virtual void unload_content() override;
        virtual void update(double gametime) override;
        virtual void draw() override;
    };
} // namespace tilegame::worldscene