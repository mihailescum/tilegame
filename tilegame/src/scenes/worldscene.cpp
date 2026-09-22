#include "worldscene.hpp"

#include "components/renderable2d.hpp"
#include "components/depth.hpp"
#include "components/animation.hpp"

namespace tilegame::scenes
{
    WorldScene::WorldScene(engine::Game &game)
        : tilegame::Scene(game),
          _system_render(*this, _registry),
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
          _system_weather(*this, _registry),
          _system_lightning(*this, _registry),
          _system_collision_detection(*this, _registry),
          _system_messagebox(*this, _registry),
          _system_interaction(*this, _registry),
          _system_facing(*this, _registry),
          _system_sprite_orientation(*this, _registry)
    {
    }

    void WorldScene::initialize()
    {
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
        _system_messagebox.initialize();
    }

    void WorldScene::load_content()
    {
        _system_daytime.load_content();
        _system_lightning.load_content(); // Shares Daytime's shader, so must run after Daytime::load_content()

        _system_player.load_content();
        _system_particle.load_content();

        _system_camera.load_content();
        _system_weather.load_content(); // Pins its precipitation entity to the camera, so must run after Camera::load_content()

        _system_render.load_content();

        // Runs the global Lua scripts (content/scripts/daytime.lua, weather.lua), which
        // immediately raise events like SetWeatherPrecipitationEvent/ShakeCameraHorizontalEvent/
        // SetLightningEvent (see systems::Script::load_content()) - must run last, once every
        // system above has already created whatever entities/listeners those events are
        // delivered to.
        _system_script.load_content();
    }

    void WorldScene::unload_content()
    {
    }

    void WorldScene::update(const engine::GameTime &update_time)
    {
        _system_timer.update(update_time);
        _system_particle.update(update_time);

        _system_player.update(update_time);              // Can generate direction of a colliding entity
        _system_movement_controller.update(update_time); // Transforms directions to movement instruction; also writes NPC Direction from Target
        _system_facing.update(update_time);              // Persists latest non-zero Direction (player + NPCs) - must run after both Direction writers above
        _system_interaction.update(update_time);         // Reads Facing - must run after Facing
        _system_sprite_orientation.update(update_time);  // Reads Facing, swaps Animation/Sprite to match - must run after Facing

        _system_collision_detection.update(update_time); // Resolves all collisions on movement direction level
        _system_movement.update(update_time);            // Actually updated the positions

        _system_animation.update(update_time);
        _system_pin.update(update_time);

        _system_camera.update(update_time);

        _system_daytime.update(update_time);
        _system_weather.update(update_time);
        _system_lightning.update(update_time);
    }

    void WorldScene::end_update()
    {
        _system_movement_controller.end_update();
    }

    void WorldScene::begin_draw()
    {
    }

    void WorldScene::draw(const engine::GameTime &draw_time)
    {
        _system_render.draw(draw_time);
    }

    void WorldScene::end_draw(const engine::GameTime &draw_time)
    {
    }
} // namespace tilegame::worldscene