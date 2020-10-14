#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace engine
{
    class TileLayer
    {
    private:
        static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

        std::vector<unsigned> data;

        std::vector<unsigned> parseVector(const std::vector<unsigned> &data);

    public:
        TileLayer() = default;
        TileLayer(const TileLayer &layer) = default;

        bool loadFromJsonDocument(const nlohmann::json &document);
        const std::vector<unsigned> &getData() const;
    };
} // namespace engine