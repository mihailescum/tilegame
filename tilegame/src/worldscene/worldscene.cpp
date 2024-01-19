#include "worldscene.hpp"

#include "tilegame.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/scenenode.hpp"
#include "components/animation.hpp"

namespace tilegame::worldscene
{
    WorldScene::WorldScene(Tilegame &game)
        : tilegame::Scene(game),
          _postprocessor(game.graphicsdevice()),
          _system_render(*this, _registry, game.spritebatch()),
          _system_map(*this, _registry),
          _system_camera(*this, _registry),
          _system_player(*this, _registry),
          _system_movement(*this, _registry),
          _system_movement_controller(*this, _registry),
          _system_pin(*this, _registry),
          _system_animation(*this, _registry),
          _system_script(*this, _registry),
          _system_timer(*this, _registry),
          _system_particle(*this, _registry),
          _system_daytime(*this, _registry),
          _system_collision_detection(*this, _registry)
    {
    }

    void WorldScene::initialize()
    {
        _system_daytime.initialize();

        _system_pin.initialize();
        _system_render.initialize();
        _system_animation.initialize();
        _system_script.initialize();
        _system_particle.initialize();

        _system_player.initialize();
        _system_camera.initialize();
        _system_movement.initialize();
        _system_movement_controller.initialize();

        _system_collision_detection.initialize();

        int postprocessor_result = _postprocessor.initialize();
        if (!postprocessor_result)
        {
            throw "Failed to initilize PostProcessor";
        }
    }

    void WorldScene::load_content()
    {
        _system_daytime.load_content();

        _system_map.load_content();
        _system_player.load_content();
        _system_particle.load_content();

        _system_camera.load_content();

        _system_render.load_content();

        // Setup PostProcessor
        _postprocessor.add_color_attachments(2);

        auto &daytime_shader = _game.resource_manager().get<engine::Shader>("daytime_shader");
        engine::graphics::PostProcessingEffect daytime_effect(_game.graphicsdevice(), daytime_shader);
        daytime_effect.input_textures().push_back(std::ref(_postprocessor.color_attachment_at(0)));
        daytime_effect.add_color_attachments(1);

        auto &blend_shader = _game.resource_manager().get<engine::Shader>("blend_shader");
        engine::graphics::PostProcessingEffect blend_effect(_game.graphicsdevice(), blend_shader);
        blend_effect.input_textures().push_back(std::ref(daytime_effect.color_attachment_at(0)));
        blend_effect.input_textures().push_back(std::ref(_postprocessor.color_attachment_at(1)));
        blend_effect.add_color_attachments(1);

        _postprocessor.effects().push_back(std::move(daytime_effect));
        _postprocessor.effects().push_back(std::move(blend_effect));
    }

    void WorldScene::unload_content()
    {
    }

    void WorldScene::update(const engine::GameTime &update_time)
    {
        _system_daytime.update(update_time);

        _system_timer.update(update_time);
        _system_particle.update(update_time);

        _system_player.update(update_time);              // Can generate direction of a colliding entity
        _system_script.update(update_time);              // Can generate direction of a colliding
        _system_movement_controller.update(update_time); // Transforms directions to movement instruction

        _system_collision_detection.update(update_time); // Resolves all collisions on movement direction level
        _system_movement.update(update_time);            // Actually updated the positions

        _system_animation.update(update_time);
        _system_pin.update(update_time);

        _system_camera.update(update_time);
    }

    void WorldScene::end_update()
    {
        _system_timer.end_update();
        _system_movement_controller.end_update();
        _system_movement.end_update();
    }

    void WorldScene::begin_draw()
    {
    }

    void WorldScene::draw(const engine::GameTime &draw_time)
    {
        _postprocessor.begin_scene();

        // Draw the scene
        _system_render.draw(draw_time);

        _postprocessor.end_scene();
        _postprocessor.apply_effects(draw_time);
    }

    void WorldScene::end_draw(const engine::GameTime &draw_time)
    {
    }
} // namespace tilegame::worldscene