#ifndef __ENGINE_MAPLAYER_H__
#define __ENGINE_MAPLAYER_H__

#include "tinyxml2.h"

namespace engine
{
    class MapLayer
    {
    public:
        virtual void loadFromXMLElement(const tinyxml2::XMLElement *element) = 0;
    };
} // namespace engine

#endif