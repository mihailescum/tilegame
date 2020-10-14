#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>

#include "core/rectangle.hpp"

namespace engine
{
    struct TileLayerComponent
    {
    public:
        TileLayerComponent() = default;
        TileLayerComponent(const TileLayerComponent &layer) = default;
        TileLayerComponent(const std::vector<unsigned> &data, const int widthInTiles, const int heightInTiles);

        std::vector<unsigned> data;
        int widthInTiles;
        int heightInTiles;
    };
} // namespace engine
