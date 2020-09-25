#ifndef __ENGINE_OBJECTLAYER_H__
#define __ENGINE_OBJECTLAYER_H__

#include "tinyxml2.h"

namespace engine
{
    class ObjectLayer
    {
    private:
    public:
        void loadFromXMLElement(const tinyxml2::XMLElement *element);
    };
} // namespace engine

#endif