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

        this->texture = std::unique_ptr<Texture2D>(new Texture2D());
        this->texture->loadTexture(path + imageSource, GL_RGBA, false);

        for (int y = 0; y < this->rows; y++)
        {
            for (int x = 0; x < this->columns; x++)
            {
                Tile tile;
                tile.sourceRect = Rectangle(
                    x * this->tileWidth,
                    y * this->tileHeight, 
                    (x + 1) * this->tileWidth,
                    (y + 1) * this->tileHeight);
            }
        }
    }

    const Texture2D &Tileset::getTexture() const
    {
        return *this->texture;
    }

} // namespace engine