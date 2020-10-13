#include "worldscene/contentsystem.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    void ContentSystem::initialize() {}

    void ContentSystem::loadContent() const
    {
        engine::Map *map1 = resourceManager.loadResource<engine::Map>("map1", "content/world/map1.tmx", this);
        this->createMapEntities(*map1);
    }

    void ContentSystem::unloadContent() const
    {
    }

    void ContentSystem::createMapEntities(engine::Map &map) const
    {
        const std::vector<std::pair<const engine::Tileset *, const int>> &tilesets = map.getTilesets();
        const std::vector<std::unique_ptr<const engine::TileLayer>> &layers = map.getLayers();

        engine::TilesetComponent tilesetComponent(tilesets.at(0).second, tilesets.at(0).first);

        double q = 1;
        for (auto &&layer : layers)
        {
            engine::Entity entity = scene.createEntity();
            entity.add<engine::TileLayerComponent>(layer->getData(), map.getWidth(), map.getHeight());
            entity.add<engine::TilesetComponent>(tilesetComponent);
            entity.add<engine::PositionComponent>(glm::vec2(0.0));
            entity.add<engine::RenderComponent>(map.getWidth() * map.getTileWidth(), map.getHeight() * map.getTileHeight(), 1 - q);
            q *= 0.5;
        } 
    }
} // namespace tilegame::worldscene