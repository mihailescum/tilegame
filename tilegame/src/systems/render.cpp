#include "render.hpp"

#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/camera.hpp"
#include "components/daytime.hpp"
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
        _font = _scene.game().resource_manager().load_resource<engine::graphics::SpriteFont>("font_default", "content/fonts/default.json");

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

        if (_needs_sorting)
        {
            sort_renderables();
            _needs_sorting = false;
        }

        const auto view_renderable = _registry.view<const components::Transform, const components::Renderable2D>(entt::exclude<components::Inactive>);
        const auto view_sprites = _registry.view<const components::Renderable2D, const components::Sprite>(entt::exclude<components::Inactive>);
        const auto view_tilelayers = _registry.view<const components::Renderable2D, const components::TileLayer>(entt::exclude<components::Inactive>);
        const auto view_particle_pools = _registry.view<const components::Renderable2D, const components::ParticlePool>(entt::exclude<components::Inactive>);

        const auto camera_entity = _registry.ctx().get<entt::entity>(components::CAMERA_ENTITY_ID);
        const auto &camera = _registry.get<const components::Camera>(camera_entity);
        const engine::Rectangle &visible_bounds = camera.visible_bounds;

        _spritebatch_luminosity_shader->use();
        _spritebatch_luminosity_shader->set("night_amount", _registry.ctx().get<float>(components::NIGHT_AMOUNT_ID));

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
                draw_tilelayer(transform, tilelayer_component, visible_bounds);
            }
            else if (view_particle_pools.contains(render_entity))
            {
                auto &pool_component = view_particle_pools.get<const components::ParticlePool>(render_entity);
                draw_particles(pool_component, visible_bounds);
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

            if (const auto shape_circle = std::get_if<engine::Circle>(&collider.shape))
            {
                glm::vec2 pos = position + shape_circle->origin - shape_circle->radius;
                engine::Rectangle dest_rect(pos, glm::vec2(shape_circle->radius * 2));
                _spritebatch.draw(_circle_tex, dest_rect, nullptr, shape_color);
            }
            else if (const auto shape_rect = std::get_if<engine::Rectangle>(&collider.shape))
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

        //
        // Dialog box (screen-space, unaffected by any camera and by the post-processing effects
        // above - drawn straight onto the default framebuffer)
        //

        const auto &message_box_state = _registry.ctx().get<const components::MessageBoxState>();
        if (!message_box_state.lines.empty())
        {
            draw_message_box(message_box_state);
        }
        // Only once the message's last line is on screen - i.e. no more pages left to page
        // through via Enter - does the options box (if any) appear.
        if (!message_box_state.options.empty() && message_box_state.lines.size() <= static_cast<std::size_t>(tilegame::messagebox_layout::VISIBLE_LINES))
        {
            draw_options_box(message_box_state);
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

    void Render::draw_tilelayer(const components::Transform &transform, const components::TileLayer &tilelayer, const engine::Rectangle &visible_bounds)
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

            _spritebatch.draw(data.textures, dest_rect, &data.source_rect, engine::Color::WHITE);
        }
    }

    void Render::draw_particles(const components::ParticlePool &pool, const engine::Rectangle &visible_bounds)
    {
        // TODO this should be outside of the loop to avoid refeching for every emitter
        const auto particles_entities = _registry.view<components::Particle, components::Sprite, components::Transform>(entt::exclude<components::Inactive>);

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

            _spritebatch.draw(sprite.textures, dest_rect, &source_rect, particle.color);
        }
    }

    void Render::draw_message_box(const components::MessageBoxState &state)
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
        for (const auto &line : state.lines)
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

    void Render::draw_options_box(const components::MessageBoxState &state)
    {
        using namespace tilegame::messagebox_layout;

        const auto &viewport = _scene.game().graphicsdevice().viewport();
        const int cell_height = _font->cell_height();

        const float message_box_height = BOX_PADDING * 2 + cell_height * VISIBLE_LINES;
        const float message_box_y = viewport.dimensions.y - BOX_MARGIN_BOTTOM - message_box_height;

        const float box_width = viewport.dimensions.x * OPTIONS_BOX_WIDTH_RATIO;
        const float box_height = BOX_PADDING * 2 + cell_height * static_cast<float>(state.options.size());
        const float box_x = viewport.dimensions.x - box_width;
        const float box_y = message_box_y - OPTIONS_BOX_MARGIN_BOTTOM - box_height;

        _spritebatch.begin(true);
        const engine::Rectangle box_rect(glm::vec2(box_x, box_y), glm::vec2(box_width, box_height));
        _spritebatch.draw(_rect_tex, box_rect, nullptr, engine::Color(0.0f, 0.0f, 0.0f, 0.75f));
        _spritebatch.end();

        _text_spritebatch.begin(true);
        for (std::size_t option_index = 0; option_index < state.options.size(); option_index++)
        {
            const float line_bottom = box_y + BOX_PADDING + (option_index + 1) * cell_height;
            const std::string prefix = option_index == state.selected_option ? "> " : "  ";
            _text_spritebatch.draw_text(prefix + state.options[option_index], *_font, glm::vec2(box_x + BOX_PADDING, line_bottom), engine::Color::WHITE);
        }
        _text_spritebatch.end();
    }
}