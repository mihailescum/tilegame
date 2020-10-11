#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>

#include "core/rectangle.hpp"

namespace engine
{
    struct TileLayerComponent
    {
    private:
        static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

        static std::vector<unsigned> parseCSV(const std::string &csv);

    public:
        TileLayerComponent() = default;
        TileLayerComponent(const TileLayerComponent &layer) = default;
        TileLayerComponent(const std::vector<unsigned> &data, const int widthInTiles, const int heightInTiles);

        std::vector<unsigned> data;
        int widthInTiles;
        int heightInTiles;

        static TileLayerComponent loadFromXMLElement(const tinyxml2::XMLElement *element);
    };
} // namespace engine
