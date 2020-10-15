#pragma once

#include <string>

namespace engine
{
    struct MapObject
    {
    protected:
        MapObject() {}

    public:
        int x;
        int y;
        int width;
        int height;
    };

    struct NpcObject : public MapObject
    {
        NpcObject() = default;
        std::string id;
    };
} // namespace engine