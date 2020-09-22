#include "core/graphicsdevice.hpp"

#include <sstream>
#include "core/log.hpp"

namespace engine
{
    GraphicsDevice::GraphicsDevice(const Window &window) : defaultViewport(0, 0, window.getWindowWidth(), window.getWindowHeight()), viewport(defaultViewport)
    {
    }

    int GraphicsDevice::create()
    {
        clear_mask = GL_COLOR_BUFFER_BIT;// | GL_DEPTH_BUFFER_BIT;

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            std::stringstream error;
            error << "Unable to initialize glad" << std::endl;
            Log::e(error.str());
            return 0;
        }

        this->setViewport(defaultViewport);
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

    Viewport &GraphicsDevice::getViewport() const
    {
        return this->viewport;
    }

    void GraphicsDevice::clear(const Color &color) const
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(clear_mask);
    }
} // namespace engine