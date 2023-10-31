#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class RenderSystem : public System
    {
    private:
        engine::SpriteBatch &_spritebatch;

    public:
        RenderSystem(engine::Scene &scene, entt::registry &registry, tilegame::SceneGraphNode &scene_graph_root, engine::SpriteBatch &spritebatch);

        void draw();
    };
} // namespace tilegame