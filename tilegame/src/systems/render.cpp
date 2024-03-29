#include "render.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/camera.hpp"
#include "components/inactive.hpp"
#include "components/particle.hpp"
#include "components/collider.hpp"

namespace tilegame::systems
{
    Render::Render(tilegame::Scene &scene, entt::registry &registry, engine::graphics::SpriteBatch<engine::Texture2DContainer<2>> &spritebatch) : System(scene, registry), _spritebatch(spritebatch)
    {
    }

    void Render::initialize()
    {
        needs_sorting(_registry, entt::null);

        _registry.on_construct<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
        _registry.on_update<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
        _registry.on_destroy<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
    }

    void Render::load_content()
    {
        _spritebatch_luminosity_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "spritebatch_luminosity_shader",
            "content/shaders/spritebatch_luminosity",
            "content/shaders/spritebatch_luminosity.vert", "", "content/shaders/spritebatch_luminosity.frag");
        _spritebatch_luminosity_shader->use();
        _spritebatch_luminosity_shader->set("Texture", 0);
        _spritebatch_luminosity_shader->set("TextureLuminosity", 1);

        const engine::Texture2D *rect_tex = _scene.game().resource_manager().load_resource<engine::Texture2D>("white_rect", "content/textures/white_rect.png");
        const engine::Texture2D *circle_tex = _scene.game().resource_manager().load_resource<engine::Texture2D>("white_circle", "content/textures/white_circle.png");

        _rect_tex = {rect_tex, rect_tex};
        _circle_tex = {circle_tex, circle_tex};
    }

    void Render::draw(const engine::GameTime &draw_time)
    {
        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(static_cast<glm::vec4>(engine::Color::CORNFLOWER_BLUE)));

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
            _spritebatch.begin(camera.transform, true, _spritebatch_luminosity_shader);

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

        //
        // Debug drawing of collision shapes
        //

        const auto view_collision_shapes = _registry.view<const components::Transform, const components::Collider, const components::Renderable2D>(entt::exclude<components::Inactive>);
        const auto view_tilelayer_shapes = _registry.view<const components::Transform, const components::TileLayer, const components::Renderable2D>(entt::exclude<components::Inactive>);

        for (const auto &&[camera_entity, camera] : cameras.each())
        {
            _spritebatch.begin(camera.transform, true);

            engine::Color shape_color(0.4, 0.16, 0.93, 0.7);
            for (const auto &&[render_entity, transform, collider] : view_collision_shapes.each())
            {
                const auto &position = transform.position;

                if (const auto shape_circle = dynamic_cast<engine::Circle *>(collider.shape.get()))
                {
                    glm::vec2 pos = position + shape_circle->origin - shape_circle->radius;
                    engine::Rectangle dest_rect(pos, glm::vec2(shape_circle->radius * 2));
                    _spritebatch.draw(_circle_tex, dest_rect, nullptr, shape_color);
                }
                else if (const auto shape_rect = dynamic_cast<engine::Rectangle *>(collider.shape.get()))
                {
                    glm::vec2 pos = position + shape_rect->position;
                    engine::Rectangle dest_rect(pos, shape_rect->dimensions);
                    _spritebatch.draw(_rect_tex, dest_rect, nullptr, shape_color);
                }
            }
            engine::Color shape_color_tiles(0.93, 0.7, 0.16, 0.7);
            for (const auto &&[render_entity, transform, tilelayer] : view_tilelayer_shapes.each())
            {
                const auto &position = transform.position;

                for (const auto &data : tilelayer.tile_data)
                {
                    if (!data.textures)
                    {
                        continue;
                    }

                    if (const auto shape_circle = dynamic_cast<const engine::Circle *>(data.collision_shape))
                    {
                        glm::vec2 pos = position + data.destination_rect.position + shape_circle->origin - shape_circle->radius;
                        engine::Rectangle dest_rect(pos, glm::vec2(shape_circle->radius * 2));
                        _spritebatch.draw(_circle_tex, dest_rect, nullptr, shape_color_tiles);
                    }
                    else if (const auto shape_rect = dynamic_cast<const engine::Rectangle *>(data.collision_shape))
                    {
                        glm::vec2 pos = position + data.destination_rect.position + shape_rect->position;
                        engine::Rectangle dest_rect(pos, shape_rect->dimensions);
                        _spritebatch.draw(_rect_tex, dest_rect, nullptr, shape_color_tiles);
                    }
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
        const auto &position = transform.position;
        const auto &source_rect = sprite.source_rect;
        const engine::Rectangle dest_rect(position, source_rect.dimensions);
        _spritebatch.draw(sprite.textures, dest_rect, &source_rect, engine::Color::WHITE);
    }

    void Render::draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer) const
    {
        for (const auto &data : tilelayer.tile_data)
        {
            if (!data.textures)
            {
                continue;
            }

            _spritebatch.draw(data.textures, data.destination_rect + transform.position, &data.source_rect, engine::Color::WHITE);
        }
    }

    void Render::draw_particles(const components::ParticlePool &pool) const
    {
        // TODO this should be outside of the loop to avoid refeching for every emitter
        const auto particles_entities = _registry.view<components::Particle, components::Sprite, components::Transform>(entt::exclude<components::Inactive>);

        for (size_t i = 0; i < pool.first_dead_particle; i++)
        {
            const auto &[particle, sprite, transform] = particles_entities.get(pool.container[i]);

            const auto &position = transform.position;
            const auto &source_rect = sprite.source_rect;
            const engine::Rectangle dest_rect(position, source_rect.dimensions);
            _spritebatch.draw(sprite.textures, dest_rect, &source_rect, particle.color);
        }
    }
}