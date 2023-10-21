#include "worldscene.hpp"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "tilegame.hpp"
#include "components/worldtransform.hpp"
#include "components/renderable2d.hpp"

namespace tilegame::worldscene
{
    WorldScene::WorldScene(Tilegame &game) : engine::Scene(game), _rendersystem(*this, _registry, game.get_spritebatch())
    {
    }

    void WorldScene::initialize()
    {
    }

    void WorldScene::load_content()
    {
        auto &resource_manager = _game.get_resource_manager();

        auto test_tex = resource_manager.load_resource<engine::Texture2D>("test_tex", "content/textures/tileset1.png");

        const auto test_entity = _registry.create();
        _registry.emplace<tilegame::components::WorldTransform>(test_entity, glm::vec2(-20.0, -50.0));
        _registry.emplace<tilegame::components::Renderable2D>(test_entity, *test_tex);

        auto test_map = resource_manager.load_resource<engine::tilemap::TileMap>("test_map", "content/maps/map1.tmx");
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