#ifndef __ENGINE_GRAPHICSDEVICE_H__
#define __ENGINE_GRAPHICSDEVICE_H__

#include "glad/glad.h"

#include "core/viewport.hpp"
#include "core/window.hpp"
#include "core/color.hpp"

namespace engine
{
    class GraphicsDevice
    {
    public:
        GraphicsDevice(const Window &window);
        ~GraphicsDevice() {}
        int create();
        void setViewport(const Viewport &viewport);
        Viewport &getViewport() const;
        void clear(const Color &color) const;

    private:
        GLbitfield clear_mask;
        Viewport defaultViewport;
        Viewport &viewport;
    };
} // namespace engine

#endif