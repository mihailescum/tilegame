#include "world/tileset.hpp"

#include <string>
#include <filesystem>
#include <fstream>
#include "nlohmann/json.hpp"

#include "core/resourcemanager.hpp"
#include "core/log.hpp"

namespace engine
{
    void Tileset::unload_resource() {}

    const std::unique_ptr<nlohmann::json> Tileset::loadJsonDocument() const
    {
        std::unique_ptr<nlohmann::json> result = std::make_unique<nlohmann::json>();

        std::ifstream fileStream(this->resourcePath, std::ifstream::in);
        if (!fileStream.is_open())
        {
            return nullptr;
        }
        else
        {
            fileStream >> *result;
        }
        return result;
    }

    bool Tileset::load_resource(ResourceManager &_resource_manager, va_list args)
    {
        std::unique_ptr<nlohmann::json> jsonDocument = this->loadJsonDocument();
        if (!jsonDocument)
        {
            Log::e("File could not be loaded (file: ", this->resourcePath, ").");
            return false;
        }

        this->name = jsonDocument->value("name", "");
        this->tileWidth = jsonDocument->value("tilewidth", 0);
        if (this->tileWidth <= 0)
        {
            Log::e("Tile width (", this->tileWidth, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->tileHeight = jsonDocument->value("tileheight", 0);
        if (this->tileHeight <= 0)
        {
            Log::e("Tile height (", this->tileHeight, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->tileCount = jsonDocument->value("tilecount", 0);
        if (this->tileCount <= 0)
        {
            Log::e("Tile count (", this->tileCount, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->columns = jsonDocument->value("columns", 0);
        if (this->columns <= 0)
        {
            Log::e("Number of columns (", this->columns, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->rows = this->tileCount / this->columns;
;
        std::string imageSource = jsonDocument->value("image", "");
        if (imageSource.empty())
        {
            Log::e("Image (", imageSource, ") has to be a valid path (file: ", this->resourcePath, ").");
            return false;
        }

        std::filesystem::path imagePath = std::filesystem::canonical(this->resourcePath.parent_path() / imageSource);
        this->texture = _resource_manager.load_resource<Texture2D>(this->_resource_name + "texture", imagePath, true);

        for (int y = 0; y < this->rows; y++)
        {
            for (int x = 0; x < this->columns; x++)
            {
                Tile tile;
                tile.sourceRect = Rectangle(
                    x * this->tileWidth,
                    y * this->tileHeight,
                    this->tileWidth,
                    this->tileHeight);
                this->tiles.push_back(tile);
            }
        }

        return true;
    }

    const Texture2D &Tileset::getTexture() const { return *this->texture; }
    const Tile &Tileset::getTile(const unsigned index) const { return this->tiles[index]; }
    unsigned Tileset::getTileWidth() const { return this->tileWidth; }
    unsigned Tileset::getTileHeight() const { return this->tileHeight; }

} // namespace engine