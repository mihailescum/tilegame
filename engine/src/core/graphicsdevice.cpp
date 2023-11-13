#include "core/graphicsdevice.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine
{
    GraphicsDevice::GraphicsDevice(const Window &window) : _viewport(0, 0, window.width(), window.height())
    {
    }

    int GraphicsDevice::create()
    {
        _clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            Log::e("Unable to initialize glad");
            return 0;
        }

        viewport(_viewport);
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        return 1;
    }

    void GraphicsDevice::viewport(const Viewport &viewport)
    {
        _viewport = viewport;
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    void GraphicsDevice::clear(const Color &color) const
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(_clear_mask);
    }
} // namespace engine