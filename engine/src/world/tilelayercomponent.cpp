#include "world/tilelayercomponent.hpp"

namespace engine
{
    TileLayerComponent::TileLayerComponent(const std::vector<unsigned> &data, const int widthInTiles, const int heightInTiles)
        : data(data), widthInTiles(widthInTiles), heightInTiles(heightInTiles)
    {
    }
} // namespace engine