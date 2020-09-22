#ifndef __ENGINE_TILELAYER_H__
#define __ENGINE_TILELAYER_H__

#include <vector>
#include <string>
#include "tinyxml2.h"

namespace engine
{
    class TileLayer
    {
    private:
        std::vector<unsigned> data;
        void parseCSV(const std::string &csv);

    public:
        TileLayer() {}
        ~TileLayer() {}
        void loadFromXMLElement(const tinyxml2::XMLElement *element);
        const std::vector<unsigned> &getData() const;
    };
} // namespace engine

#endif