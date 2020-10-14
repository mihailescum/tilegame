#pragma once

#include <string>

namespace engine
{
    struct MapObject
    {
    protected:
        MapObject(const int x, const int y) : x(x), y(y) {}

    public:
        const int x;
        const int y;
    };

    struct NpcObject : public MapObject
    {
        std::string id;
    };
} // namespace engine