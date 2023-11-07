#pragma once

namespace engine::tilemap
{
    struct Tile
    {
        int ID;
        Tile() : Tile(0) {}
        Tile(int id) : ID(id) {}
    };
} // namespace engine::tilemap
