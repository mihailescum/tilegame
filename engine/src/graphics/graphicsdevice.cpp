#include "graphics/graphicsdevice.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine::graphics
{
    GraphicsDevice::GraphicsDevice(const Window &window) : _viewport(glm::ivec2(0), window.dimensions()), _clear_mask(0)
    {
    }

    int GraphicsDevice::create()
    {
        // Depth testing itself is enabled/disabled per SpriteBatch begin()/end() (see
        // graphics::DepthMode), not globally here - only some batches (the opaque world-content
        // pass) use it. Clearing GL_DEPTH_BUFFER_BIT on a framebuffer with no depth attachment is
        // a no-op, so including it here unconditionally is safe for every render target.
        _clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            Log::e("Unable to initialize glad");
            return 0;
        }

        viewport(_viewport);

        return 1;
    }

    void GraphicsDevice::viewport(const Viewport &viewport)
    {
        _viewport = viewport;
        glViewport(viewport.position.x, viewport.position.y, viewport.dimensions.x, viewport.dimensions.y);
    }

    void GraphicsDevice::clear(const Color &color) const
    {
        glClearColor(color.r(), color.g(), color.b(), color.a());
        glClear(_clear_mask);
    }
} // namespace engine::graphics