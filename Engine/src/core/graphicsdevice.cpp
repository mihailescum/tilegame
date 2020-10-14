#include "core/graphicsdevice.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine
{
    GraphicsDevice::GraphicsDevice(const Window &window) : viewport(0, 0, window.getWindowWidth(), window.getWindowHeight())
    {
    }

    int GraphicsDevice::create()
    {
        clear_mask = GL_COLOR_BUFFER_BIT;// | GL_DEPTH_BUFFER_BIT;

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            Log::e("Unable to initialize glad");
            return 0;
        }

        this->setViewport(viewport);
        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);

        return 1;
    }

    void GraphicsDevice::setViewport(const Viewport &viewport)
    {
        this->viewport = viewport;
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    const Viewport &GraphicsDevice::getViewport() const
    {
        return this->viewport;
    }

    void GraphicsDevice::clear(const Color &color) const
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(clear_mask);
    }
} // namespace engine