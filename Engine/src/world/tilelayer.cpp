#include "world/tilelayer.hpp"

#include <sstream>

namespace engine
{
    void TileLayer::loadFromXMLElement(const tinyxml2::XMLElement *element)
    {
        const tinyxml2::XMLElement *dataElement = element->FirstChildElement("data");
        std::string data = dataElement->GetText();

        this->parseCSV(data);
    }

    void TileLayer::parseCSV(const std::string &csv)
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

                int gid = std::stoi(cell);

                bool flipped_horizontally = (gid & FLIPPED_HORIZONTALLY_FLAG);
                bool flipped_vertically = (gid & FLIPPED_VERTICALLY_FLAG);
                bool flipped_diagonally = (gid & FLIPPED_DIAGONALLY_FLAG);
                gid &= ~(FLIPPED_HORIZONTALLY_FLAG |
                         FLIPPED_VERTICALLY_FLAG |
                         FLIPPED_DIAGONALLY_FLAG);

                this->data.push_back(gid);
            }
        }
    }

    const std::vector<unsigned> &TileLayer::getData() const { return this->data; }
} // namespace engine