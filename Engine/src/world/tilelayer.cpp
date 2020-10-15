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

        const nlohmann::json &propertiesDocument = document.value("properties", nlohmann::json::array());
        std::string objectId;
        for (const nlohmann::json &propertyDocument : propertiesDocument)
        {
            const std::string propertyName = propertyDocument.value("name", "");
            const std::string propertyType = propertyDocument.value("type", "");
            if (propertyName == "object_id")
            {
                if (propertyType == "string")
                    objectId = propertyDocument.value("value", "");
                else if (propertyType == "int")
                    objectId = propertyDocument.value("value", -1);
            }
        }
        this->objectId = objectId;

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
    const std::string &TileLayer::getObjectId() const { return this->objectId; }
    bool TileLayer::isVisible() const { return this->visible; }
} // namespace engine