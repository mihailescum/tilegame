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
        GLbitfield _clear_mask;
        Viewport _viewport;

    public:
        GraphicsDevice(const Window &window);

        int create();
        void clear(const Color &color) const;

        // TODO review if getter/setter are necessary
        const Viewport &viewport() const { return _viewport; }
        void viewport(const Viewport &viewport);
    };
} // namespace engine
