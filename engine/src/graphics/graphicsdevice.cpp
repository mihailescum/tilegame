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
        _clear_mask = GL_COLOR_BUFFER_BIT;

        int gladInitRes = gladLoadGL();
        if (!gladInitRes)
        {
            Log::e("Unable to initialize glad");
            return 0;
        }

        viewport(_viewport);
        // glDisable(GL_CULL_FACE);
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

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