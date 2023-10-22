#include "rendersystem.hpp"

#include "components/renderable2d.hpp"
#include "components/worldtransform.hpp"

namespace tilegame::systems
{
    RenderSystem::RenderSystem(engine::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void RenderSystem::draw()
    {
        _registry.sort<tilegame::components::WorldTransform>([](const auto &lhs, const auto &rhs)
                                                             { return lhs.position.z < rhs.position.z; });
        auto view = _registry.view<const tilegame::components::WorldTransform, const tilegame::components::Renderable2D>();

        _spritebatch.begin(true);
        for (const auto entity : view)
        {
            auto &&[world, render] = view.get<const tilegame::components::WorldTransform, const tilegame::components::Renderable2D>(entity);
            const auto &tex = render.texture.get();
            const auto &source_rect = render.source_rect;

            engine::Rectangle dest_rect(world.position.x, world.position.y, source_rect.width, source_rect.height);
            _spritebatch.draw(tex, dest_rect, &source_rect, engine::Color::WHITE);
        }
        _spritebatch.end();
    }
}