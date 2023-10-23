#include "core/graphicsdevice.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine
{
    GraphicsDevice::GraphicsDevice(const Window &window) : viewport(0, 0, window.get_width(), window.get_height())
    {
    }

    int GraphicsDevice::create()
    {
        clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            Log::e("Unable to initialize glad");
            return 0;
        }

        set_viewport(viewport);
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        return 1;
    }

    void GraphicsDevice::set_viewport(const Viewport &viewport)
    {
        this->viewport = viewport;
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    void GraphicsDevice::clear(const Color &color) const
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(clear_mask);
    }
} // namespace engine