#pragma once

#include "glad/glad.h"

#include "core/viewport.hpp"
#include "core/window.hpp"
#include "core/color.hpp"

namespace engine::graphics
{
    /**
     * @brief Thin wrapper around the low-level OpenGL device state (viewport, clear
     * color/mask). Owns the GL context setup via glad and is passed by reference to
     * higher-level rendering classes (SpriteBatch, PostProcessor, ...) that need to
     * query the current viewport or issue clears.
     */
    class GraphicsDevice
    {
    private:
        GLbitfield _clear_mask;
        Viewport _viewport;

    public:
        GraphicsDevice(const Window &window);

        /** @brief Loads GL function pointers via glad and applies the initial viewport. Must be called once a valid GL context exists, before any rendering. */
        int create();
        void clear(const Color &color) const;

        // TODO review if getter/setter are necessary
        const Viewport &viewport() const { return _viewport; }
        void viewport(const Viewport &viewport);
    };
} // namespace engine::graphics
