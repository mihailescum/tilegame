#include "map/tilelayer.hpp"

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

                this->data.push_back(std::stoi(cell));
            }
        }
    }

    const std::vector<unsigned> &TileLayer::getData() const { return this->data; }
} // namespace engine