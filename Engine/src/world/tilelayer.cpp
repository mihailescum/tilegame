#include "world/tilelayer.hpp"

#include <sstream>

namespace engine
{
    bool TileLayer::loadFromXMLElement(const tinyxml2::XMLElement *element)
    {
        const tinyxml2::XMLElement *dataElement = element->FirstChildElement("data");
        const std::string dataString = dataElement->GetText();
        this->data = this->parseCSV(dataString);

        return true;
    }

    std::vector<unsigned> TileLayer::parseCSV(const std::string &csv)
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