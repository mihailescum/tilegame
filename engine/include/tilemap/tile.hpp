#pragma once

namespace engine::tilemap
{
    class Tileset;

    struct Tile
    {
        int gid;
        int width;
        int height;
        const Tileset *tileset;

        Tile() : Tile(0, 0, 0, nullptr) {}
        Tile(int id, int width, int height, const Tileset *tileset) : gid(id),
                                                                      width(width),
                                                                      height(height),
                                                                      tileset(tileset) {}
    };
} // namespace engine::tilemap
