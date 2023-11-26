#include "render.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/camera.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    RenderSystem::RenderSystem(tilegame::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void RenderSystem::initialize()
    {
        needs_sorting();

        _registry.on_construct<components::Ordering>().connect<&systems::RenderSystem::needs_sorting>(*this);
        _registry.on_update<components::Ordering>().connect<&systems::RenderSystem::needs_sorting>(*this);
        _registry.on_destroy<components::Ordering>().connect<&systems::RenderSystem::needs_sorting>(*this);
    }

    void RenderSystem::draw(const engine::GameTime &draw_time)
    {
        if (_needs_sorting)
        {
            sort_renderables();
            _needs_sorting = false;
        }

        const auto view_renderable = _registry.view<components::Transform, components::Renderable2D>(entt::exclude<tilegame::components::Inactive>);
        const auto view_sprites = _registry.view<components::Renderable2D, components::Sprite>(entt::exclude<tilegame::components::Inactive>);
        const auto view_tilelayers = _registry.view<components::Renderable2D, components::TileLayer>(entt::exclude<tilegame::components::Inactive>);

        const auto cameras = _registry.view<components::Camera>(entt::exclude<tilegame::components::Inactive>);

        for (const auto &&[camera_entity, camera] : cameras.each())
        {
            _spritebatch.begin(camera.transform, true);

            // Since Renderable2D is only a tag, it does not show up in the view
            for (const auto &&[render_entity, transform] : view_renderable.each())
            {
                if (view_sprites.contains(render_entity))
                {
                    const auto &sprite_component = view_sprites.get<components::Sprite>(render_entity);
                    draw_sprite(transform, sprite_component);
                }
                else if (view_tilelayers.contains(render_entity))
                {
                    const auto &tilelayer_component = view_tilelayers.get<components::TileLayer>(render_entity);
                    draw_tilelayer(transform, tilelayer_component);
                }
            }
            _spritebatch.end();
        }
    }

    void RenderSystem::sort_renderables() const
    {
        // TODO can we make this more efficient somehow?
        _registry.sort<components::Ordering>(
            [](const auto &lhs, const auto &rhs)
            { return lhs.z < rhs.z; },
            entt::insertion_sort());
        _registry.sort<components::Renderable2D, components::Ordering>();
    }

    void RenderSystem::draw_sprite(const components::Transform &transform, const components::Sprite &sprite) const
    {
        const auto &position_global = transform.position_global;
        const auto &source_rect = sprite.source_rect;
        const engine::Rectangle dest_rect(position_global.x, position_global.y, source_rect.width, source_rect.height);
        _spritebatch.draw(sprite.texture, dest_rect, &source_rect, engine::Color::WHITE);
    }

    void RenderSystem::draw_tilelayer(const components::Transform &transform, const components::TileLayer &tile_layer) const
    {
        for (const auto data : tile_layer.tile_data)
        {
            _spritebatch.draw(data.texture, data.destination_rect + transform.position_global, &data.source_rect, engine::Color::WHITE);
        }
    }
}