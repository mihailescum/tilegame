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
          _system_render(*this, _registry, game.spritebatch()),
          _system_map(*this, _registry),
          _system_camera(*this, _registry),
          _system_player(*this, _registry),
          _system_movement(*this, _registry),
          _system_scenegraph(*this, _registry),
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
        _game.postprocessing_enabled() = true;
        _system_daytime.initialize();

        _system_scenegraph.initialize();
        _system_render.initialize();
        _system_animation.initialize();
        _system_script.initialize();
        _system_particle.initialize();

        _system_player.initialize();
        _system_camera.initialize();
        _system_movement.initialize();

        _system_collision_detection.initialize();
    }

    void WorldScene::load_content()
    {
        _system_daytime.load_content();

        _system_map.load_content();
        _system_player.load_content();
        _system_particle.load_content();

        _system_camera.load_content();
    }

    void WorldScene::unload_content()
    {
        _system_scenegraph.unload_content();
    }

    void WorldScene::update(const engine::GameTime &update_time)
    {
        _system_daytime.update(update_time);

        _system_timer.update(update_time);
        _system_particle.update(update_time);

        _system_player.update(update_time);
        _system_script.update(update_time);

        _system_movement.update(update_time);
        _system_collision_detection.update(update_time);

        _system_animation.update(update_time);
        _system_scenegraph.update(update_time);

        _system_camera.update(update_time);
    }

    void WorldScene::end_update()
    {
        _system_timer.end_update();
        _system_movement.end_update();
    }

    void WorldScene::draw(const engine::GameTime &draw_time)
    {
        _system_render.draw(draw_time);
    }
} // namespace tilegame::worldscene