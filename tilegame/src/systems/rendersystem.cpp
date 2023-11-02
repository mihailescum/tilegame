#include "rendersystem.hpp"

#include "components/renderable2d.hpp"
#include "components/transform.hpp"
#include "components/renderlayer.hpp"
#include "components/camera.hpp"

namespace tilegame::systems
{
    RenderSystem::RenderSystem(tilegame::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void RenderSystem::draw()
    {
        _registry.sort<tilegame::components::RenderLayer>([](const auto &lhs, const auto &rhs)
                                                          { return lhs.z_index < rhs.z_index; });
        const auto view = _registry.view<tilegame::components::RenderLayer>();
        const auto view_renderable = _registry.view<tilegame::components::Transform, tilegame::components::Renderable2D>();

        auto cameras = _registry.view<tilegame::components::Camera>();

        for (auto &&[entity, camera] : cameras.each())
        {
            _spritebatch.begin(camera.transform, true);
            for (auto &&[entity, render_layer] : view.each())
            {
                for (const auto render_entity : render_layer.children)
                {
                    const auto &&[transform, render] = view_renderable.get<tilegame::components::Transform, tilegame::components::Renderable2D>(render_entity);

                    const auto &tex = render.texture;
                    const auto &source_rect = render.source_rect;

                    const auto &position_global = transform.position_global;
                    engine::Rectangle dest_rect(position_global.x, position_global.y, source_rect.width, source_rect.height);
                    _spritebatch.draw(tex, dest_rect, &source_rect, engine::Color::WHITE);
                }
            }
            _spritebatch.end();
        }
    }
}