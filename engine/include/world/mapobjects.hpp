#pragma once

#include <string>

namespace engine
{
    struct MapObject
    {
    protected:
        MapObject() {}

    public:
        virtual ~MapObject() {}

        double x;
        double y;
        double width;
        double height;
    };

    struct NpcObject : public MapObject
    {
        NpcObject() = default;
        std::string objectId;
    };
} // namespace engine