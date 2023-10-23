#include "rendersystem.hpp"

#include "components/renderable2d.hpp"
#include "components/worldtransform.hpp"
#include "components/renderlayer.hpp"

namespace tilegame::systems
{
    RenderSystem::RenderSystem(engine::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void RenderSystem::draw()
    {
        _registry.sort<tilegame::components::RenderLayer>([](const auto &lhs, const auto &rhs)
                                                          { return lhs.z_index < rhs.z_index; });
        const auto view = _registry.view<tilegame::components::RenderLayer>();
        const auto view_renderable = _registry.view<tilegame::components::WorldTransform, tilegame::components::Renderable2D>();

        _spritebatch.begin(true);
        for (const auto entity : view)
        {
            const auto &render_layer = view.get<tilegame::components::RenderLayer>(entity);

            for (const auto render_entity : render_layer.children)
            {
                const auto &&[world, render] = view_renderable.get<tilegame::components::WorldTransform, tilegame::components::Renderable2D>(render_entity);

                const auto &tex = render.texture.get();
                const auto &source_rect = render.source_rect;

                engine::Rectangle dest_rect(world.position.x, world.position.y, source_rect.width, source_rect.height);
                _spritebatch.draw(tex, dest_rect, &source_rect, engine::Color::WHITE);
            }
        }
        _spritebatch.end();
    }
}