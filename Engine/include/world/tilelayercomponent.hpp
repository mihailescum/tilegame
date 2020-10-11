#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>

namespace engine
{
    struct TileLayerComponent
    {
    private:
        static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

        static TileLayerComponent parseCSV(const std::string &csv);

    public:
        TileLayerComponent() = default;
        TileLayerComponent(const TileLayerComponent &layer) = default;
        TileLayerComponent(const std::vector<unsigned> &data);

        std::vector<unsigned> data;
        static TileLayerComponent loadFromXMLElement(const tinyxml2::XMLElement *element);
    };
} // namespace engine
