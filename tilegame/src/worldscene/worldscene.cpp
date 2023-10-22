#include "worldscene.hpp"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "tilegame.hpp"
#include "components/worldtransform.hpp"
#include "components/renderable2d.hpp"

namespace tilegame::worldscene
{
    WorldScene::WorldScene(Tilegame &game)
        : engine::Scene(game),
          _rendersystem(*this, _registry, game.get_spritebatch()),
          _mapsystem(*this, _registry)
    {
    }

    void WorldScene::initialize()
    {
    }

    void WorldScene::load_content()
    {
        _mapsystem.create_map_entity_from_file("map1", "content/maps/map1.tmx");
    }

    void WorldScene::unload_content()
    {
    }

    void WorldScene::update(const engine::GameTime &update_time)
    {
    }

    void WorldScene::draw(const engine::GameTime &draw_time)
    {
        _rendersystem.draw();
    }
} // namespace tilegame::worldscene