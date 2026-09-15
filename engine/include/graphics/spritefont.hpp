#pragma once

#include <glm/glm.hpp>

#include "core/resource.hpp"
#include "core/rectangle.hpp"
#include "core/texture2d.hpp"

namespace engine::graphics
{
    /**
     * @brief A monospace bitmap font: a grid of glyph cells within a single Texture2D, described by
     * a small JSON descriptor rather than hardcoded metrics, so new fonts/atlases can be added as
     * content without touching engine code. Loaded as a Resource (e.g. `content/fonts/default.json`).
     *
     * Expected JSON shape (the texture path is relative to the descriptor's own directory):
     * @code
     * {
     *     "texture": "font.png",
     *     "first_char": 32,
     *     "columns": 16,
     *     "cell_width": 21,
     *     "cell_height": 37
     * }
     * @endcode
     */
    class SpriteFont : public Resource
    {
    private:
        const Texture2D *_texture;
        char _first_char;
        int _columns;
        int _rows;
        int _cell_width;
        int _cell_height;

    public:
        SpriteFont();

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        const Texture2D &texture() const { return *_texture; }
        int cell_width() const { return _cell_width; }
        int cell_height() const { return _cell_height; }

        /// Source rectangle of `c`'s glyph cell within texture(); characters outside the atlas fall back to the space glyph.
        Rectangle glyph_source_rect(char c) const;
    };
} // namespace engine::graphics
