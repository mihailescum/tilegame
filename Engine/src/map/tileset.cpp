#include "map/tileset.hpp"

#include <string>
#include <filesystem>
#include "tinyxml2.h"

#include "core/resourcemanager.hpp"

namespace engine
{
    void Tileset::unloadResource() { }

    bool Tileset::loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filename.c_str());

        const tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->name = root->Attribute("name");
        this->tileWidth = root->UnsignedAttribute("tilewidth");
        this->tileHeight = root->UnsignedAttribute("tileheight");
        this->tileCount = root->UnsignedAttribute("tilecount");
        this->columns = root->UnsignedAttribute("columns");
        this->rows = this->tileCount / this->columns;

        const tinyxml2::XMLElement *image = root->FirstChildElement("image");
        std::string imageSource = image->Attribute("source");
        std::filesystem::path imagePath = std::filesystem::canonical(std::filesystem::absolute(filename).parent_path() / imageSource);

        this->texture = resourceManager.loadResource<Texture2D>(this->resourceName + "texture", imagePath, true);

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

        return true;
    }

    const Texture2D &Tileset::getTexture() const { return *this->texture; }
    const Tile &Tileset::getTile(const unsigned index) const { return this->tiles[index]; }
    unsigned Tileset::getTileWidth() const { return this->tileWidth; }
    unsigned Tileset::getTileHeight() const { return this->tileHeight; }

} // namespace engine