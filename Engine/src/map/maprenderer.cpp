#include "map/maprenderer.hpp"
#include "map/map.hpp"

namespace engine
{
    void MapRenderer::Draw(SpriteBatch &spriteBatch) const
    {
        spriteBatch.begin();

        const Texture2D &texture = map.getTilesetInfo().tileset->getTexture();
        unsigned firstGid = map.getTilesetInfo().firstGid;

        for (unsigned y = 0; y < this->map.getHeight(); y++)
        {
            for (unsigned x = 0; x < this->map.getWidth(); x++)
            {
                unsigned index = x + y * this->map.getWidth();
                unsigned gid = map.getData()[index];
                const Rectangle &sourceRect = map.getTilesetInfo().tileset->getTile(gid - firstGid).sourceRect;
                Rectangle destinationRect(
                    x * map.getTilesetInfo().tileset->getTileWidth(),
                    y * map.getTilesetInfo().tileset->getTileHeight(),
                    map.getTilesetInfo().tileset->getTileWidth(),
                    map.getTilesetInfo().tileset->getTileHeight());

                spriteBatch.draw(texture, destinationRect, &sourceRect, Color::White);
            }
        }

        spriteBatch.end();
    }
} // namespace engine