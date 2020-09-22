#include "map/maprenderer.hpp"
#include "map/map.hpp"

namespace engine
{
    void MapRenderer::initialize()
    {
        this->tilesetInfo = &map.getTilesetInfo();
        this->mapWidth = map.getWidth();
        this->mapHeight = map.getHeight();
    }

    void MapRenderer::draw(SpriteBatch &spriteBatch) const
    {
        spriteBatch.begin(true);

        const Texture2D &texture = tilesetInfo->tileset->getTexture();
        unsigned firstGid = tilesetInfo->firstGid;

        for (const auto &layer : map.getLayers())
        {
            for (unsigned y = 0; y < this->mapHeight; y++)
            {
                for (unsigned x = 0; x < this->mapWidth; x++)
                {
                    unsigned index = x + y * this->mapWidth;
                    unsigned gid = layer->getData()[index];
                    if (gid < firstGid)
                        continue;

                    const Rectangle &sourceRect = this->tilesetInfo->tileset->getTile(gid - firstGid).sourceRect;
                    Rectangle destinationRect(
                        x * this->tilesetInfo->tileset->getTileWidth(),
                        y * this->tilesetInfo->tileset->getTileHeight(),
                        this->tilesetInfo->tileset->getTileWidth(),
                        this->tilesetInfo->tileset->getTileHeight());

                    spriteBatch.draw(texture, destinationRect, &sourceRect, Color::White);
                }
            }
        }

        spriteBatch.end();
    }
} // namespace engine