#include "worldscene.hpp"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

#include "../tilegame.hpp"

namespace tilegame::worldscene
{
    WorldScene::WorldScene(Tilegame &game) : engine::Scene(game)
    {
    }

    void WorldScene::initialize()
    {
        this->createSystems();
    }

    void WorldScene::createSystems()
    {
    }

    void WorldScene::load_content()
    {
        _test_tex = _game.get_resource_manager().load_resource<engine::Texture2D>("test", "content/textures/tileset1.png");
    }

    void WorldScene::unload_content()
    {
    }

    void WorldScene::update(const double deltaTime)
    {
    }

    void WorldScene::draw()
    {
        auto &spriteBatch = get_spritebatch();

        spriteBatch.begin(false);
        spriteBatch.draw(*_test_tex, engine::Rectangle(0, 0, 800, 600), engine::Color::WHITE);
        spriteBatch.end();
    }

    engine::SpriteBatch &WorldScene::get_spritebatch()
    {
        return ((Tilegame &)_game).get_spritebatch();
    }
} // namespace tilegame::worldscene