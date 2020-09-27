#pragma once

#include "tinyxml2.h"

namespace engine
{
    class MapLayer
    {
    public:
        virtual void loadFromXMLElement(const tinyxml2::XMLElement *element) = 0;
    };
} // namespace engine
