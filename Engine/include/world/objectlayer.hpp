#pragma once

#include <tinyxml2.h>

namespace engine
{
    class ObjectLayer
    {
    private:
        void parseNPC(const tinyxml2::XMLElement *object, const int x, const int y, const int width, const int height);

    public:
        void loadFromXMLElement(const tinyxml2::XMLElement *element);
    };
} // namespace engine
