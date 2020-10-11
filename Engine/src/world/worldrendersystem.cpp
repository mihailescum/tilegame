#include "world/worldrendersystem.hpp"

#include "core/texture2D.hpp"
#include "scene/components/positioncomponent.hpp"
#include "scene/components/rendercomponent.hpp"
#include "world/map.hpp"

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
        auto drawables = registry.view<RenderComponent, PositionComponent>();

        spriteBatch.begin(camera, true);
        for (auto entity : drawables)
        {
            // Todo: Do frustum culling here
            if (registry.has<TilesetComponent, TileLayerComponent>(entity))
            {
                this->drawTileLayer(spriteBatch, entity);
            }
        }
        spriteBatch.end();
    }

    void WorldRenderSystem::drawTileLayer(SpriteBatch &spriteBatch, const entt::entity &entity) const
    {
        const auto [tileset, tileLayer, position] = registry.get<TilesetComponent, TileLayerComponent, PositionComponent>(entity);

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
} // namespace engine