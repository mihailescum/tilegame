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

        float x;
        float y;
        float width;
        float height;
    };

    struct NpcObject : public MapObject
    {
        NpcObject() = default;
        std::string objectId;
    };
} // namespace engine