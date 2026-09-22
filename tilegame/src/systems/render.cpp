#include "render.hpp"

#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

#include "components/renderable2d.hpp"
#include "components/camera.hpp"
#include "components/depthorigin.hpp"
#include "components/daytime.hpp"
#include "components/inactive.hpp"
#include "components/particle.hpp"
#include "components/collider.hpp"

namespace tilegame::systems
{
    Render::Render(tilegame::Scene &scene, entt::registry &registry)
        : System(scene, registry), _spritebatch(scene.game().graphicsdevice()), _postprocessor(scene.game().graphicsdevice())
    {
    }

    void Render::initialize()
    {
        _spritebatch.create();

        int postprocessor_result = _postprocessor.initialize();
        if (!postprocessor_result)
        {
            throw std::runtime_error("Failed to initilize PostProcessor");
        }
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

        // Setup PostProcessor: day/night tint, then a two-pass (horizontal+vertical) Gaussian
        // blur of the luminosity pass to turn it into actual bloom, then additive blend it back
        // onto the tinted scene.
        _postprocessor.add_color_attachments(2);

        auto &daytime_shader = _scene.game().resource_manager().get<engine::Shader>("daytime_shader");
        engine::graphics::PostProcessingEffect daytime_effect(_scene.game().graphicsdevice(), daytime_shader);
        daytime_effect.input_textures().push_back(std::ref(_postprocessor.color_attachment_at(0)));
        daytime_effect.add_color_attachments(1);

        // Two independent Shader resources (not shared with each other) compiled from the same
        // gaussian_blur.frag source, so each can hold its own fixed `direction` uniform - if
        // both passes shared one GL program, setting `direction` for the second pass would
        // overwrite the first's before either ever actually draws with it.
        const glm::vec2 texel_size(1.0f / _scene.game().graphicsdevice().viewport().dimensions.x, 1.0f / _scene.game().graphicsdevice().viewport().dimensions.y);

        auto &blur_h_shader = *_scene.game().resource_manager().load_resource<engine::Shader>(
            "bloom_blur_h_shader",
            "content/shaders/gaussian_blur",
            "content/shaders/quad.vert", "", "content/shaders/gaussian_blur.frag");
        blur_h_shader.use();
        blur_h_shader.set("scene", 0);
        blur_h_shader.set("direction", glm::vec2(1.0f, 0.0f));
        blur_h_shader.set("texel_size", texel_size);
        engine::graphics::PostProcessingEffect blur_h_effect(_scene.game().graphicsdevice(), blur_h_shader);
        blur_h_effect.input_textures().push_back(std::ref(_postprocessor.color_attachment_at(1)));
        blur_h_effect.add_color_attachments(1);

        auto &blur_v_shader = *_scene.game().resource_manager().load_resource<engine::Shader>(
            "bloom_blur_v_shader",
            "content/shaders/gaussian_blur",
            "content/shaders/quad.vert", "", "content/shaders/gaussian_blur.frag");
        blur_v_shader.use();
        blur_v_shader.set("scene", 0);
        blur_v_shader.set("direction", glm::vec2(0.0f, 1.0f));
        blur_v_shader.set("texel_size", texel_size);
        engine::graphics::PostProcessingEffect blur_v_effect(_scene.game().graphicsdevice(), blur_v_shader);
        blur_v_effect.input_textures().push_back(std::ref(blur_h_effect.color_attachment_at(0)));
        blur_v_effect.add_color_attachments(1);

        auto &blend_shader = _scene.game().resource_manager().get<engine::Shader>("blend_shader");
        engine::graphics::PostProcessingEffect blend_effect(_scene.game().graphicsdevice(), blend_shader);
        blend_effect.input_textures().push_back(std::ref(daytime_effect.color_attachment_at(0)));
        blend_effect.input_textures().push_back(std::ref(blur_v_effect.color_attachment_at(0)));
        blend_effect.add_color_attachments(1);

        _postprocessor.effects().push_back(std::move(daytime_effect));
        _postprocessor.effects().push_back(std::move(blur_h_effect));
        _postprocessor.effects().push_back(std::move(blur_v_effect));
        _postprocessor.effects().push_back(std::move(blend_effect));
    }

    void Render::draw(const engine::GameTime &draw_time)
    {
        _postprocessor.begin_scene();

        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(static_cast<glm::vec4>(engine::Color::CORNFLOWER_BLUE)));

        // Depth (unlike Transform/Renderable2D) isn't a filter here - TileLayer entities carry
        // no Depth of their own any more (see draw_tilelayer()), only Sprite/ParticlePool ones
        // do, fetched below via view_sprites/view_particle_pools instead.
        const auto view_renderable = _registry.view<const components::Transform, const components::Renderable2D>(entt::exclude<components::Inactive>);
        const auto view_sprites = _registry.view<const components::Renderable2D, const components::Sprite, const components::Depth>(entt::exclude<components::Inactive>);
        const auto view_tilelayers = _registry.view<const components::Renderable2D, const components::TileLayer>(entt::exclude<components::Inactive>);
        const auto view_particle_pools = _registry.view<const components::Renderable2D, const components::ParticlePool, const components::Depth>(entt::exclude<components::Inactive>);

        const auto camera_entity = _registry.ctx().get<entt::entity>(components::CAMERA_ENTITY_ID);
        const auto &camera = _registry.get<const components::Camera>(camera_entity);
        const engine::Rectangle &visible_bounds = camera.visible_bounds;
        const float depth_origin_y = _registry.get<const components::DepthOrigin>(camera_entity).y;

        _spritebatch_luminosity_shader->use();
        _spritebatch_luminosity_shader->set("night_amount", _registry.ctx().get<float>(components::NIGHT_AMOUNT_ID));

        // Opaque pass: every tile and sprite, alpha blending off. No draw-order bookkeeping of
        // any kind here - the GPU depth test (DepthMode::TestAndWrite) resolves overlapping
        // draws per pixel regardless of which order this loop happens to visit entities in.
        _spritebatch_luminosity_shader->set("discard_transparent", true);
        _spritebatch.begin(camera.transform, false, _spritebatch_luminosity_shader, engine::graphics::DepthMode::TestAndWrite);
        for (const auto &&[render_entity, transform] : view_renderable.each())
        {
            if (view_sprites.contains(render_entity))
            {
                draw_sprite(transform, view_sprites.get<const components::Sprite>(render_entity), view_sprites.get<const components::Depth>(render_entity), depth_origin_y);
            }
            else if (view_tilelayers.contains(render_entity))
            {
                draw_tilelayer(transform, view_tilelayers.get<const components::TileLayer>(render_entity), visible_bounds, depth_origin_y);
            }
        }
        _spritebatch.end();

        // Transparent pass: e.g. weather. Depth-tested against whatever the opaque pass left
        // behind (DepthMode::TestOnly - no depth write), real alpha blending, always last.
        _spritebatch_luminosity_shader->set("discard_transparent", false);
        _spritebatch.begin(camera.transform, true, _spritebatch_luminosity_shader, engine::graphics::DepthMode::TestOnly);
        for (const auto &&[render_entity, transform] : view_renderable.each())
        {
            if (view_particle_pools.contains(render_entity))
            {
                draw_particles(transform, view_particle_pools.get<const components::ParticlePool>(render_entity), view_particle_pools.get<const components::Depth>(render_entity), visible_bounds, depth_origin_y);
            }
        }
        _spritebatch.end();

        //
        // Debug drawing of collision shapes
        //

        const auto view_collision_shapes = _registry.view<const components::Transform, const components::Collider, const components::Renderable2D>(entt::exclude<components::Inactive>);
        const auto view_tilelayer_shapes = _registry.view<const components::Transform, const components::TileLayer, const components::Renderable2D>(entt::exclude<components::Inactive>);

        _spritebatch.begin(camera.transform, true);

        engine::Color shape_color(0.4, 0.16, 0.93, 0.7);
        for (const auto &&[render_entity, transform, collider] : view_collision_shapes.each())
        {
            const auto &position = transform.position;

            if (const auto shape_circle = std::get_if<engine::Circle>(&collider()))
            {
                glm::vec2 pos = position + shape_circle->origin - shape_circle->radius;
                engine::Rectangle dest_rect(pos, glm::vec2(shape_circle->radius * 2));
                _spritebatch.draw(_circle_tex, dest_rect, nullptr, shape_color);
            }
            else if (const auto shape_rect = std::get_if<engine::Rectangle>(&collider()))
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

                if (!(data.destination_rect + position).intersects(visible_bounds))
                {
                    continue;
                }

                if (data.collision_shape)
                {
                    if (const auto shape_circle = std::get_if<engine::Circle>(&(*data.collision_shape)))
                    {
                        glm::vec2 pos = position + data.destination_rect.position + shape_circle->origin - shape_circle->radius;
                        engine::Rectangle dest_rect(pos, glm::vec2(shape_circle->radius * 2));
                        _spritebatch.draw(_circle_tex, dest_rect, nullptr, shape_color_tiles);
                    }
                    else if (const auto shape_rect = std::get_if<engine::Rectangle>(&(*data.collision_shape)))
                    {
                        glm::vec2 pos = position + data.destination_rect.position + shape_rect->position;
                        engine::Rectangle dest_rect(pos, shape_rect->dimensions);
                        _spritebatch.draw(_rect_tex, dest_rect, nullptr, shape_color_tiles);
                    }
                }
            }
        }

        _spritebatch.end();

        _postprocessor.end_scene();
        _postprocessor.apply_effects(draw_time);
    }

    void Render::draw_sprite(const components::Transform &transform, const components::Sprite &sprite, const components::Depth &depth, float depth_origin_y)
    {
        const auto &position = transform.position;
        const auto &source_rect = sprite.source_rect;
        const engine::Rectangle dest_rect(position, source_rect.dimensions);
        // Ground-contact row (a character's feet).
        const float z = compute_z(depth(), position.y + source_rect.dimensions.y, depth_origin_y);
        _spritebatch.draw(sprite.textures, dest_rect, &source_rect, engine::Color::WHITE, z);
    }

    void Render::draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer, const engine::Rectangle &visible_bounds, float depth_origin_y)
    {
        for (const auto &data : tilelayer.tile_data)
        {
            if (!data.textures)
            {
                continue;
            }

            const engine::Rectangle dest_rect = data.destination_rect + transform.position;
            if (!dest_rect.intersects(visible_bounds))
            {
                continue;
            }

            const float z = compute_z(data.depth, data.reference_y, depth_origin_y);
            _spritebatch.draw(data.textures, dest_rect, &data.source_rect, engine::Color::WHITE, z);
        }
    }

    void Render::draw_particles(const components::Transform &emitter_transform, const components::ParticlePool &pool, const components::Depth &depth, const engine::Rectangle &visible_bounds, float depth_origin_y)
    {
        // TODO this should be outside of the loop to avoid refeching for every emitter
        const auto particles_entities = _registry.view<components::Particle, components::Sprite, components::Transform>(entt::exclude<components::Inactive>);

        // A single depth value per emitter, not one per particle - see the class comment.
        const float z = compute_z(depth(), emitter_transform.position.y, depth_origin_y);

        for (size_t i = 0; i < pool.first_dead_particle; i++)
        {
            const auto &[particle, sprite, transform] = particles_entities.get(pool.container[i]);

            const auto &position = transform.position;
            const auto &source_rect = sprite.source_rect;
            // Scaled around the sprite's center rather than its top-left corner, so shrinking a
            // particle doesn't also visibly drift it towards `position`.
            const glm::vec2 scaled_dimensions = source_rect.dimensions * particle.scale;
            const glm::vec2 scaled_position = position - (scaled_dimensions - source_rect.dimensions) * 0.5f;
            const engine::Rectangle dest_rect(scaled_position, scaled_dimensions);
            if (!dest_rect.intersects(visible_bounds))
            {
                continue;
            }

            _spritebatch.draw(sprite.textures, dest_rect, &source_rect, particle.color, z);
        }
    }

    float Render::compute_z(float depth_base, float world_y, float depth_origin_y) const
    {
        return depth_base + (world_y - depth_origin_y) * DEPTH_FINE_SCALE;
    }
}
