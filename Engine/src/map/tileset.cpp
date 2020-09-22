#include "map/tileset.hpp"

#include <string>
#include "tinyxml2.h"

namespace engine
{
    std::string getFileName(const std::string &s)
    {

        char sep = '/';

#ifdef _WIN32
        sep = '\\';
#endif

        size_t i = s.rfind(sep, s.length());
        if (i != std::string::npos)
        {
            return (s.substr(i + 1, s.length() - i));
        }

        return s;
    }

    void Tileset::loadFromFile(const std::string filename, const std::string folder)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filename.c_str());

        tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->name = root->Attribute("name");
        this->tileWidth = std::stoi(root->Attribute("tilewidth"));
        this->tileHeight = std::stoi(root->Attribute("tileheight"));
        this->tileCount = std::stoi(root->Attribute("tilecount"));
        this->columns = std::stoi(root->Attribute("columns"));
        this->rows = this->tileCount / this->columns;

        tinyxml2::XMLElement *image = root->FirstChildElement("image");
        std::string imageSource = image->Attribute("source");
        if (!folder.empty()) //If a folder is passed, we take only the source filename
            imageSource = getFileName(imageSource);
        this->texture.loadTexture(folder + imageSource, GL_RGBA, false);

        for (int y = 0; y < this->rows; y++)
        {
            for (int x = 0; x < this->columns; x++)
            {
                Tile tile;
                tile.sourceRect = Rectangle(
                    x * this->tileWidth,
                    y * this->tileHeight, 
                    (x + 1) * this->tileWidth,
                    (y + 1) * this->tileHeight);
            }
        }
    }

    const Texture2D &Tileset::getTexture() const
    {
        return this->texture;
    }

} // namespace engine