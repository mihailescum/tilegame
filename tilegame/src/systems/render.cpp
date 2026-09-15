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
    Render::Render(tilegame::Scene &scene, entt::registry &registry)
        : System(scene, registry), _spritebatch(scene.game().graphicsdevice()), _text_spritebatch(scene.game().graphicsdevice()), _postprocessor(scene.game().graphicsdevice())
    {
    }

    void Render::initialize()
    {
        needs_sorting(_registry, entt::null);

        _registry.on_construct<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
        _registry.on_update<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);
        _registry.on_destroy<components::Ordering>().connect<&systems::Render::needs_sorting>(*this);

        _spritebatch.create();
        _text_spritebatch.create();

        int postprocessor_result = _postprocessor.initialize();
        if (!postprocessor_result)
        {
            throw "Failed to initilize PostProcessor";
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
        _font = _scene.game().resource_manager().load_resource<engine::graphics::SpriteFont>("font_default", "content/fonts/default.json");

        _rect_tex = {rect_tex, rect_tex};
        _circle_tex = {circle_tex, circle_tex};

        // Setup PostProcessor: day/night tint, then additive blend with the luminosity pass
        _postprocessor.add_color_attachments(2);

        auto &daytime_shader = _scene.game().resource_manager().get<engine::Shader>("daytime_shader");
        engine::graphics::PostProcessingEffect daytime_effect(_scene.game().graphicsdevice(), daytime_shader);
        daytime_effect.input_textures().push_back(std::ref(_postprocessor.color_attachment_at(0)));
        daytime_effect.add_color_attachments(1);

        auto &blend_shader = _scene.game().resource_manager().get<engine::Shader>("blend_shader");
        engine::graphics::PostProcessingEffect blend_effect(_scene.game().graphicsdevice(), blend_shader);
        blend_effect.input_textures().push_back(std::ref(daytime_effect.color_attachment_at(0)));
        blend_effect.input_textures().push_back(std::ref(_postprocessor.color_attachment_at(1)));
        blend_effect.add_color_attachments(1);

        _postprocessor.effects().push_back(std::move(daytime_effect));
        _postprocessor.effects().push_back(std::move(blend_effect));
    }

    void Render::draw(const engine::GameTime &draw_time)
    {
        _postprocessor.begin_scene();

        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(static_cast<glm::vec4>(engine::Color::CORNFLOWER_BLUE)));

        if (_needs_sorting)
        {
            sort_renderables();
            _needs_sorting = false;
        }

        const auto view_renderable = _registry.view<const components::Transform, const components::Renderable2D>(entt::exclude<components::Inactive>);
        const auto view_sprites = _registry.view<const components::Renderable2D, const components::Sprite>(entt::exclude<components::Inactive>);
        const auto view_tilelayers = _registry.view<const components::Renderable2D, const components::TileLayer>(entt::exclude<components::Inactive>);
        const auto view_particle_pools = _registry.view<const components::Renderable2D, const components::ParticlePool>(entt::exclude<components::Inactive>);

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

        _postprocessor.end_scene();
        _postprocessor.apply_effects(draw_time);

        //
        // Dialog box (screen-space, unaffected by any camera and by the post-processing effects
        // above - drawn straight onto the default framebuffer)
        //

        const auto message_boxes = _registry.view<const components::MessageBox>();
        if (!message_boxes.empty())
        {
            const auto entity = *message_boxes.begin();
            const auto &box = message_boxes.get<const components::MessageBox>(entity);

            draw_message_box(box);
        }
    }

    void Render::sort_renderables()
    {
        // TODO can we make this more efficient somehow?
        _registry.sort<components::Ordering>(
            [](const auto &lhs, const auto &rhs)
            { return lhs() < rhs(); },
            entt::insertion_sort());
        _registry.sort<components::Renderable2D, components::Ordering>();
    }

    void Render::draw_sprite(const components::Transform &transform, const components::Sprite &sprite)
    {
        const auto &position = transform.position;
        const auto &source_rect = sprite.source_rect;
        const engine::Rectangle dest_rect(position, source_rect.dimensions);
        _spritebatch.draw(sprite.textures, dest_rect, &source_rect, engine::Color::WHITE);
    }

    void Render::draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer)
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

    void Render::draw_particles(const components::ParticlePool &pool)
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

    void Render::draw_message_box(const components::MessageBox &box)
    {
        using namespace tilegame::messagebox_layout;

        const auto &viewport = _scene.game().graphicsdevice().viewport();
        const int cell_height = _font->cell_height();

        const float box_width = static_cast<float>(viewport.dimensions.x);
        const float box_height = BOX_PADDING * 2 + cell_height * VISIBLE_LINES;
        const float box_y = viewport.dimensions.y - BOX_MARGIN_BOTTOM - box_height;

        _spritebatch.begin(true);
        const engine::Rectangle box_rect(glm::vec2(0.0f, box_y), glm::vec2(box_width, box_height));
        _spritebatch.draw(_rect_tex, box_rect, nullptr, engine::Color(0.0f, 0.0f, 0.0f, 0.75f));
        _spritebatch.end();

        _text_spritebatch.begin(true);
        int line_index = 0;
        for (const auto &line : box.lines)
        {
            if (line_index >= VISIBLE_LINES)
            {
                break;
            }

            const float line_bottom = box_y + BOX_PADDING + (line_index + 1) * cell_height;
            _text_spritebatch.draw_text(line, *_font, glm::vec2(BOX_PADDING, line_bottom), engine::Color::WHITE);

            line_index++;
        }
        _text_spritebatch.end();
    }
}