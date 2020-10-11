#include "world/tilelayercomponent.hpp"

#include <sstream>

namespace engine
{
    TileLayerComponent::TileLayerComponent(const std::vector<unsigned> &data, const int widthInTiles, const int heightInTiles)
        : data(data), widthInTiles(widthInTiles), heightInTiles(heightInTiles)
    {
    }

    TileLayerComponent TileLayerComponent::loadFromXMLElement(const tinyxml2::XMLElement *element)
    {
        const int widthInTiles = element->IntAttribute("width");
        const int heightInTiles = element->IntAttribute("height");

        const tinyxml2::XMLElement *dataElement = element->FirstChildElement("data");
        const std::string dataString = dataElement->GetText();

        const std::vector<unsigned> data = TileLayerComponent::parseCSV(dataString);

        return TileLayerComponent(data, widthInTiles, heightInTiles);
    }

    std::vector<unsigned> TileLayerComponent::parseCSV(const std::string &csv)
    {
        std::vector<unsigned> data;

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

                int gid = std::stoi(cell);

                bool flipped_horizontally = (gid & FLIPPED_HORIZONTALLY_FLAG);
                bool flipped_vertically = (gid & FLIPPED_VERTICALLY_FLAG);
                bool flipped_diagonally = (gid & FLIPPED_DIAGONALLY_FLAG);
                gid &= ~(FLIPPED_HORIZONTALLY_FLAG |
                         FLIPPED_VERTICALLY_FLAG |
                         FLIPPED_DIAGONALLY_FLAG);

                data.push_back(gid);
            }
        }

        return data;
    }
} // namespace engine