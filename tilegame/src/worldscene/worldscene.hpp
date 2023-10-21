#pragma once

#include <string>
#include <unordered_map>

#include "entt.hpp"

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
        virtual void update(const engine::GameTime &update_time) override;
        virtual void draw(const engine::GameTime &draw_time) override;
    };
} // namespace tilegame::worldscene