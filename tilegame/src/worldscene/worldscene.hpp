#pragma once

#include <string>
#include <unordered_map>

#include <entt/entity/registry.hpp>

#include "engine.hpp"
#include "systems/rendersystem.hpp"
#include "systems/mapsystem.hpp"
#include "systems/camerasystem.hpp"

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
        tilegame::systems::RenderSystem _system_render;
        tilegame::systems::MapSystem _system_map;
        tilegame::systems::CameraSystem _system_camera;

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