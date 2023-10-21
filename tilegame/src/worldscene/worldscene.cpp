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

    void WorldScene::loadContent()
    {
        testTex = game.getResourceManager().loadResource<engine::Texture2D>("test", "content/textures/tileset1.png");
    }

    void WorldScene::unloadContent()
    {
    }

    void WorldScene::processInput()
    {
    }

    void WorldScene::update(const double deltaTime)
    {
    }

    void WorldScene::draw()
    {
        auto &spriteBatch = getSpriteBatch();

        spriteBatch.begin(false);
        spriteBatch.draw(*testTex, engine::Rectangle(0, 0, 800, 600), engine::Color::White);
        spriteBatch.end();
    }

    engine::SpriteBatch &WorldScene::getSpriteBatch()
    {
        return ((Tilegame &)game).getSpriteBatch();
    }
} // namespace tilegame::worldscene