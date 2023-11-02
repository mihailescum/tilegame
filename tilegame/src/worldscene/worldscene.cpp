#include "worldscene.hpp"

#include "tilegame.hpp"

namespace tilegame::worldscene
{
    WorldScene::WorldScene(Tilegame &game)
        : tilegame::Scene(game),
          _system_render(*this, _registry, game.get_spritebatch()),
          _system_map(*this, _registry),
          _system_camera(*this, _registry),
          _system_player(*this, _registry),
          _system_movement(*this, _registry),
          _system_scenegraph(*this, _registry)
    {
    }

    void WorldScene::initialize()
    {
        _system_scenegraph.initialize();
        _system_render.initialize();

        _system_player.initialize();
        _system_camera.initialize();
    }

    void WorldScene::load_content()
    {
        _system_player.load_content();
        _system_map.create_map_entity_from_file("map1", "content/maps/map1.tmx");
    }

    void WorldScene::unload_content()
    {
        _system_scenegraph.unload_content();
    }

    void WorldScene::update(const engine::GameTime &update_time)
    {
        _system_player.update(update_time);
        _system_movement.update(update_time);

        _system_scenegraph.update(update_time);

        _system_camera.update(update_time);
    }

    void WorldScene::draw(const engine::GameTime &draw_time)
    {
        _system_render.draw(draw_time);
    }
} // namespace tilegame::worldscene