#include "world/tilelayer.hpp"

#include <sstream>

namespace engine
{
    bool TileLayer::loadFromJsonDocument(const nlohmann::json &document)
    {
        this->visible = document.value("visible", true);
        std::vector<unsigned> data = document.value<std::vector<unsigned>>("data", {});
        if (data.empty())
            return false;

        this->data = this->parseVector(data);

        return true;
    }

    std::vector<unsigned> TileLayer::parseVector(const std::vector<unsigned> &data)
    {
        std::vector<unsigned> result;

        for (unsigned gid : data)
        {
            bool flipped_horizontally = (gid & FLIPPED_HORIZONTALLY_FLAG);
            bool flipped_vertically = (gid & FLIPPED_VERTICALLY_FLAG);
            bool flipped_diagonally = (gid & FLIPPED_DIAGONALLY_FLAG);
            gid &= ~(FLIPPED_HORIZONTALLY_FLAG |
                     FLIPPED_VERTICALLY_FLAG |
                     FLIPPED_DIAGONALLY_FLAG);

            result.push_back(gid);
        }

        return result;
    }

    const std::vector<unsigned> &TileLayer::getData() const { return this->data; }
    bool TileLayer::isVisible() const { return this->visible; }
} // namespace engine