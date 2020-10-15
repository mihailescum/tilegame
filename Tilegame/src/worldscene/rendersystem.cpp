#include "worldscene/rendersystem.hpp"

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    RenderSystem::RenderSystem(WorldScene &scene, engine::SpriteBatch &spriteBatch) : registry(scene.getRegistry()), spriteBatch(spriteBatch) {}

    void RenderSystem::initialize()
    {
    }

    void RenderSystem::draw() const
    {
        registry.sort<engine::RenderComponent>([](const auto &lhs, const auto &rhs) {
            return lhs.z < rhs.z;
        });

        auto drawables = this->registry.view<engine::RenderComponent, engine::PositionComponent, engine::VisiblityComponent>();
        auto cameras = this->registry.view<engine::CameraComponent>();
        for (auto entity : cameras)
        {
            const engine::CameraComponent &camera = cameras.get(entity);

            if (camera.viewport)
            {
                spriteBatch.begin(camera, true);
                drawables.each<engine::RenderComponent>(
                    [=](auto entity, auto const &render, const auto &pos, const auto &visibility) {
                        // Todo: Do frustum culling here
                        if (registry.has<engine::TilesetComponent, engine::TileLayerComponent>(entity))
                        {
                            this->drawTileLayer(entity);
                        }
                        if (registry.has<engine::SpriteComponent, engine::SpriteInfoComponent>(entity))
                        {
                            this->drawSprite(entity);
                        }
                    });
                spriteBatch.end();
            }
        }
    }

    void RenderSystem::drawTileLayer(const entt::entity &entity) const
    {
        const auto [tileset, tileLayer, position] = this->registry.get<engine::TilesetComponent, engine::TileLayerComponent, engine::PositionComponent>(entity);

        const engine::Texture2D &texture = tileset.tileset->getTexture();
        unsigned firstGid = tileset.firstGid;

        for (int y = 0; y < tileLayer.heightInTiles; y++)
        {
            for (int x = 0; x < tileLayer.widthInTiles; x++)
            {
                unsigned index = x + y * tileLayer.widthInTiles;
                unsigned gid = tileLayer.data[index];

                if (gid < firstGid)
                    continue;

                const engine::Rectangle &sourceRect = tileset.tileset->getTile(gid - firstGid).sourceRect;
                engine::Rectangle destinationRect(
                    position.x() + x * tileset.tileset->getTileWidth(),
                    position.y() + y * tileset.tileset->getTileHeight(),
                    tileset.tileset->getTileWidth(),
                    tileset.tileset->getTileHeight());

                spriteBatch.draw(texture, destinationRect, &sourceRect, engine::Color::White);
            }
        }
    }

    // Draws sprite centered around center of the source rectangle
    void RenderSystem::drawSprite(const entt::entity &entity) const
    {
        const auto [sprite, position] = this->registry.get<engine::SpriteComponent, engine::PositionComponent>(entity);

        const engine::Rectangle &sourceRect = sprite.sourceRectangle;
        engine::Rectangle destinationRect(
            floor(position.x()),
            floor(position.y()),
            sourceRect.width,
            sourceRect.height);
        spriteBatch.draw(*sprite.texture, destinationRect, &sourceRect, engine::Color::White);
    }
} // namespace tilegame::worldscene