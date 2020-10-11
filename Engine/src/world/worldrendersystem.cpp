#include "world/worldrendersystem.hpp"

#include "core/texture2D.hpp"
#include "scene/components/positioncomponent.hpp"
#include "scene/components/rendercomponent.hpp"
#include "scene/components/spritecomponent.hpp"
#include "scene/components/spritesheetcomponent.hpp"
#include "world/tilelayercomponent.hpp"
#include "world/tilesetcomponent.hpp"

#include "core/log.hpp"

namespace engine
{
    void WorldRenderSystem::initialize()
    {
    }

    void WorldRenderSystem::draw(SpriteBatch &spriteBatch, const CameraComponent &camera) const
    {
        registry.sort<RenderComponent>([](const auto &lhs, const auto &rhs) {
            return lhs.z < rhs.z;
        });
        auto drawables = this->registry.view<RenderComponent, PositionComponent>();

        spriteBatch.begin(camera, true);
        for (auto entity : drawables)
        {
            // Todo: Do frustum culling here
            if (registry.has<TilesetComponent, TileLayerComponent>(entity))
            {
                this->drawTileLayer(spriteBatch, entity);
            }
            if (registry.has<SpriteComponent, SpriteSheetComponent>(entity))
            {
                this->drawSprite(spriteBatch, entity);
            }
        }
        spriteBatch.end();
    }

    void WorldRenderSystem::drawTileLayer(SpriteBatch &spriteBatch, const entt::entity &entity) const
    {
        const auto [tileset, tileLayer, position] = this->registry.get<TilesetComponent, TileLayerComponent, PositionComponent>(entity);

        const Texture2D &texture = tileset.tileset->getTexture();
        unsigned firstGid = tileset.firstGid;

        for (int y = 0; y < tileLayer.heightInTiles; y++)
        {
            for (int x = 0; x < tileLayer.widthInTiles; x++)
            {
                unsigned index = x + y * tileLayer.widthInTiles;
                unsigned gid = tileLayer.data[index];

                if (gid < firstGid)
                    continue;

                const Rectangle &sourceRect = tileset.tileset->getTile(gid - firstGid).sourceRect;
                Rectangle destinationRect(
                    position.x() + x * tileset.tileset->getTileWidth(),
                    position.y() + y * tileset.tileset->getTileHeight(),
                    tileset.tileset->getTileWidth(),
                    tileset.tileset->getTileHeight());

                spriteBatch.draw(texture, destinationRect, &sourceRect, Color::White);
            }
        }
    }

    // Draws sprite centered around center of the source rectangle
    void WorldRenderSystem::drawSprite(SpriteBatch &spriteBatch, const entt::entity &entity) const
    {
        const auto [sprite, position] = this->registry.get<SpriteComponent, PositionComponent>(entity);

        const Rectangle &sourceRect = sprite.sourceRectangle;
        Rectangle destinationRect(
            floor(position.x() - sourceRect.width / 2),
            floor(position.y() - sourceRect.height / 2),
            sourceRect.width,
            sourceRect.height);
        spriteBatch.draw(*sprite.texture, destinationRect, &sourceRect, Color::White);
    }
} // namespace engine