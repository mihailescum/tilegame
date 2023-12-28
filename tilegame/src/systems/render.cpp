#include "render.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/camera.hpp"
#include "components/inactive.hpp"
#include "components/particle.hpp"

namespace tilegame::systems
{
    Render::Render(tilegame::Scene &scene, entt::registry &registry, engine::graphics::SpriteBatch &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void Render::initialize()
    {
        needs_sorting(_registry, entt::null);

        _registry.on_construct<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
        _registry.on_update<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
        _registry.on_destroy<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
    }

    void Render::draw(const engine::GameTime &draw_time)
    {
        if (_needs_sorting)
        {
            sort_renderables();
            _needs_sorting = false;
        }

        const auto view_renderable = _registry.view<const components::Transform, const components::Renderable2D>(entt::exclude<components::Inactive>);
        const auto view_sprites = _registry.view<const components::Renderable2D, const components::Sprite>(entt::exclude<components::Inactive>);
        const auto view_tilelayers = _registry.view<const components::Renderable2D, const components::TileLayer>(entt::exclude<components::Inactive>);
        const auto view_particle_pools = _registry.view<const components::Renderable2D, const components::ParticlePool>(entt ::exclude<components::Inactive>);

        const auto cameras = _registry.view<const components::Camera>(entt::exclude<components::Inactive>);

        for (const auto &&[camera_entity, camera] : cameras.each())
        {
            _spritebatch.begin(camera.transform, true);

            // Since Renderable2D is only a tag, it does not show up in the view
            for (const auto &&[render_entity, transform] : view_renderable.each())
            {
                if (view_sprites.contains(render_entity))
                {
                    auto &sprite_component = view_sprites.get<const components::Sprite>(render_entity);
                    draw_sprite(transform, sprite_component);
                }
                else if (view_tilelayers.contains(render_entity))
                {
                    auto &tilelayer_component = view_tilelayers.get<const components::TileLayer>(render_entity);
                    draw_tilelayer(transform, tilelayer_component);
                }
                else if (view_particle_pools.contains(render_entity))
                {
                    auto &pool_component = view_particle_pools.get<const components::ParticlePool>(render_entity);
                    draw_particles(pool_component);
                }
            }
            _spritebatch.end();
        }
    }

    void Render::sort_renderables() const
    {
        // TODO can we make this more efficient somehow?
        _registry.sort<components::Ordering>(
            [](const auto &lhs, const auto &rhs)
            { return lhs() < rhs(); },
            entt::insertion_sort());
        _registry.sort<components::Renderable2D, components::Ordering>();
    }

    void Render::draw_sprite(const components::Transform &transform, const components::Sprite &sprite) const
    {
        const auto &position_global = transform.position_global;
        const auto &source_rect = sprite.source_rect;
        const engine::Rectangle dest_rect(position_global.x, position_global.y, source_rect.width, source_rect.height);
        _spritebatch.draw(*sprite.texture, dest_rect, &source_rect, engine::Color::WHITE);
    }

    void Render::draw_tilelayer(const components::Transform &transform, const components::TileLayer &tile_layer) const
    {
        for (const auto &data : tile_layer.tile_data)
        {
            _spritebatch.draw(data.texture, data.destination_rect + transform.position_global, &data.source_rect, engine::Color::WHITE);
        }
    }

    void Render::draw_particles(const components::ParticlePool &pool) const
    {
        // TODO this should be outside of the loop to avoid refeching for every emitter
        const auto particles_view = _registry.view<components::Particle, components::Sprite, components::Transform>(entt::exclude<components::Inactive>);

        for (size_t i = 0; i < pool.first_dead_particle; i++)
        {
            const auto &[particle, sprite, transform] = particles_view.get(pool.container[i]);

            const auto &position_global = transform.position_global;
            const auto &source_rect = sprite.source_rect;
            const engine::Rectangle dest_rect(position_global.x, position_global.y, source_rect.width, source_rect.height);
            _spritebatch.draw(*sprite.texture, dest_rect, &source_rect, particle.color);
        }
    }
}