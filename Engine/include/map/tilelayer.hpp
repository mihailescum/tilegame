#ifndef __ENGINE_TILELAYER_H__
#define __ENGINE_TILELAYER_H__

#include <vector>
#include <string>
#include "tinyxml2.h"

#include "map/maplayer.hpp"

namespace engine
{
    class TileLayer : public MapLayer
    {
    private:
        static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

        std::vector<unsigned> data;

        void parseCSV(const std::string &csv);

    public:
        virtual void loadFromXMLElement(const tinyxml2::XMLElement *element) override;
        const std::vector<unsigned> &getData() const;
    };
} // namespace engine

#endif