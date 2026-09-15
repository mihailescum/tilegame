#include "graphics/spritefont.hpp"

#include <fstream>
#include <sstream>

#include "tileson/tileson.hpp"

#include "core/resourcemanager.hpp"
#include "core/log.hpp"

namespace engine::graphics
{
    SpriteFont::SpriteFont()
        : _texture(nullptr), _first_char(0), _columns(0), _rows(0), _cell_width(0), _cell_height(0)
    {
    }

    bool SpriteFont::load_resource(ResourceManager &resource_manager, va_list args)
    {
        std::ifstream file(_resource_path);
        if (!file)
        {
            Log::e("Failed to open sprite font descriptor. FILE: ", _resource_path);
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string err;
        const json11::Json json = json11::Json::parse(buffer.str(), err);
        if (!err.empty())
        {
            Log::e("Failed to parse sprite font descriptor. FILE: ", _resource_path, " ERROR: ", err);
            return false;
        }

        _first_char = static_cast<char>(json["first_char"].int_value());
        _columns = json["columns"].int_value();
        _cell_width = json["cell_width"].int_value();
        _cell_height = json["cell_height"].int_value();

        const auto texture_path = _resource_path.parent_path() / json["texture"].string_value();
        _texture = resource_manager.load_resource<Texture2D>(texture_path.filename(), texture_path);
        if (!_texture)
        {
            return false;
        }

        _rows = _texture->dimensions().y / _cell_height;

        return true;
    }

    void SpriteFont::unload_resource()
    {
    }

    Rectangle SpriteFont::glyph_source_rect(char c) const
    {
        int index = static_cast<int>(c) - static_cast<int>(_first_char);
        const int max_index = _columns * _rows - 1;
        if (index < 0 || index > max_index)
        {
            index = 0;
        }

        const int column = index % _columns;
        const int row = index / _columns;

        return Rectangle(
            glm::vec2(column * _cell_width, row * _cell_height),
            glm::vec2(_cell_width, _cell_height));
    }
} // namespace engine::graphics
