#pragma once

#include <tinyxml2.h>

#include "map/maplayer.hpp"

namespace engine
{
    class ObjectLayer : MapLayer
    {
    private:
        void parseNPC(const tinyxml2::XMLElement *object, const int x, const int y, const int width, const int height);

    public:
        virtual void loadFromXMLElement(const tinyxml2::XMLElement *element) override;
    };
} // namespace engine
