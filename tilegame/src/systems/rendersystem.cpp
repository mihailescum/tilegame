#include "rendersystem.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/camera.hpp"

namespace tilegame::systems
{
    RenderSystem::RenderSystem(tilegame::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void RenderSystem::initialize()
    {
        needs_sorting();

        _registry.on_construct<tilegame::components::Ordering>().connect<&tilegame::systems::RenderSystem::needs_sorting>(*this);
        _registry.on_update<tilegame::components::Ordering>().connect<&tilegame::systems::RenderSystem::needs_sorting>(*this);
        _registry.on_destroy<tilegame::components::Ordering>().connect<&tilegame::systems::RenderSystem::needs_sorting>(*this);
    }

    void RenderSystem::draw(const engine::GameTime &draw_time)
    {
        if (_needs_sorting)
        {
            sort_renderables();
            _needs_sorting = false;
        }

        const auto view_renderable = _registry.view<tilegame::components::Transform, tilegame::components::Renderable2D>();
        const auto view_sprites = _registry.view<tilegame::components::Renderable2D, tilegame::components::Sprite>();
        const auto view_tilelayers = _registry.view<tilegame::components::Renderable2D, tilegame::components::TileLayer>();

        const auto cameras = _registry.view<tilegame::components::Camera>();

        for (const auto &&[camera_entity, camera] : cameras.each())
        {
            _spritebatch.begin(camera.transform, true);

            // Since Renderable2D is only a tag, it does not show up in the view
            for (const auto &&[render_entity, transform] : view_renderable.each())
            {
                if (view_sprites.contains(render_entity))
                {
                    const auto &sprite_component = view_sprites.get<tilegame::components::Sprite>(render_entity);
                    draw_sprite(transform, sprite_component);
                }
                else if (view_tilelayers.contains(render_entity))
                {
                    const auto &tilelayer_component = view_tilelayers.get<tilegame::components::TileLayer>(render_entity);
                    draw_tilelayer(transform, tilelayer_component);
                }
            }
            _spritebatch.end();
        }
    }

    void RenderSystem::sort_renderables() const
    {
        // TODO can we make this more efficient somehow?
        _registry.sort<tilegame::components::Ordering>(
            [](const auto &lhs, const auto &rhs)
            { return lhs.z < rhs.z; });
        _registry.sort<tilegame::components::Renderable2D, tilegame::components::Ordering>();
    }

    void RenderSystem::draw_sprite(const tilegame::components::Transform &transform, const tilegame::components::Sprite &sprite) const
    {
        const auto &position_global = transform.position_global;
        const auto &source_rect = sprite.source_rect;
        const engine::Rectangle dest_rect(position_global.x, position_global.y, source_rect.width, source_rect.height);
        _spritebatch.draw(sprite.texture, dest_rect, &source_rect, engine::Color::WHITE);
    }

    void RenderSystem::draw_tilelayer(const tilegame::components::Transform &transform, const tilegame::components::TileLayer &tile_layer) const
    {
        for (const auto data : tile_layer.tile_data)
        {
            _spritebatch.draw(data.texture, data.destination_rect + transform.position_global, &data.source_rect, engine::Color::WHITE);
        }
    }
}