#include "map/map.hpp"

#include "tinyxml2.h"
#include <sstream>

#include "map/tileset.hpp"

namespace engine
{
    Map::Map()
    {
        this->mapRenderer = std::unique_ptr<MapRenderer>(new MapRenderer(*this));
    }

    void Map::loadFromFile(const std::string &path, const std::string &filename)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile((path + filename).c_str());

        tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->width = root->UnsignedAttribute("width");
        this->height = root->UnsignedAttribute("height");

        tinyxml2::XMLElement *tilesetElement = root->FirstChildElement("tileset");
        int firstGid = tilesetElement->UnsignedAttribute("firstgid");

        std::string tilesetSource = tilesetElement->Attribute("source");
        std::unique_ptr<Tileset> tileset(new Tileset());
        tileset->loadFromFile(path, tilesetSource);

        this->tilesetInfo.firstGid = firstGid;
        this->tilesetInfo.tileset = std::move(tileset);

        tinyxml2::XMLElement *layerElement = root->FirstChildElement("layer");
        tinyxml2::XMLElement *dataElement = layerElement->FirstChildElement("data");
        std::string data = dataElement->GetText();

        parseCSV(data);
    }

    void Map::parseCSV(const std::string &csv)
    {
        std::istringstream ss(csv);
        for (std::string line; std::getline(ss, line);)
        {
            if (line.empty())
                continue;

            std::istringstream liness(line);
            for (std::string cell; std::getline(liness, cell, ',');)
            {
                if (cell.empty())
                    continue;

                this->data.push_back(std::stoi(cell));
            }
        }
    }

    void Map::Draw(SpriteBatch &spriteBatch) const
    {
        this->mapRenderer->Draw(spriteBatch);
    }

    unsigned Map::getWidth() const { return this->width; }
    unsigned Map::getHeight() const { return this->height; }
    const TilesetInfo &Map::getTilesetInfo() const { return this->tilesetInfo; }
    const std::vector<unsigned> &Map::getData() const { return this->data; }
} // namespace engine