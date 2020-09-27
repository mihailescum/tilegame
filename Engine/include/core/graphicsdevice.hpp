#pragma once

#include "glad/glad.h"

#include "core/viewport.hpp"
#include "core/window.hpp"
#include "core/color.hpp"

namespace engine
{
    class GraphicsDevice
    {
    private:
        GLbitfield clear_mask;
        Viewport viewport;

    public:
        GraphicsDevice(const Window &window);

        int create();
        void clear(const Color &color) const;

        const Viewport &getViewport() const;
        void setViewport(const Viewport &viewport);
    };
} // namespace engine
