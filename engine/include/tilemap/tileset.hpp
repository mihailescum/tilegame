#pragma once

#include <vector>

#include "core/texture2d.hpp"

namespace engine::tilemap
{
    class Tileset
    {
    private:
        engine::Texture2D &_texture;
        int _first_GID;

    public:
        Tileset(engine::Texture2D &texture, int first_GID) : _texture(texture), _first_GID(first_GID) {}
    };
}