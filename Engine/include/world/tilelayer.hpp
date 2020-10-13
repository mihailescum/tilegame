#pragma once

#include <string>
#include <vector>
#include <tinyxml2.h>

namespace engine
{
    class TileLayer
    {
    private:
        static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

        std::vector<unsigned> data;

        std::vector<unsigned> parseCSV(const std::string &csv);

    public:
        TileLayer() = default;
        TileLayer(const TileLayer &layer) = default;

        bool loadFromXMLElement(const tinyxml2::XMLElement *element);
    };
} // namespace engine