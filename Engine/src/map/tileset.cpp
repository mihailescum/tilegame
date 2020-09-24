#include "map/tileset.hpp"

#include <string>
#include "tinyxml2.h"

namespace engine
{
    void Tileset::loadFromFile(const std::string &path, const std::string &filename)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile((path + filename).c_str());

        tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->name = root->Attribute("name");
        this->tileWidth = root->UnsignedAttribute("tilewidth");
        this->tileHeight = root->UnsignedAttribute("tileheight");
        this->tileCount = root->UnsignedAttribute("tilecount");
        this->columns = root->UnsignedAttribute("columns");
        this->rows = this->tileCount / this->columns;

        tinyxml2::XMLElement *image = root->FirstChildElement("image");
        std::string imageSource = image->Attribute("source");

        this->texture = std::make_unique<Texture2D>();
        //this->texture->loadTexture(path + imageSource, GL_RGBA, false);

        for (int y = 0; y < this->rows; y++)
        {
            for (int x = 0; x < this->columns; x++)
            {
                Tile tile;
                tile.sourceRect = Rectangle(
                    x * this->tileWidth,
                    y * this->tileHeight,
                    this->tileWidth,
                    this->tileHeight);
                this->tiles.push_back(tile);
            }
        }
    }

    const Texture2D &Tileset::getTexture() const { return *this->texture; }
    const Tile &Tileset::getTile(const unsigned index) const { return this->tiles[index]; }
    unsigned Tileset::getTileWidth() const { return this->tileWidth; }
    unsigned Tileset::getTileHeight() const { return this->tileHeight; }

} // namespace engine