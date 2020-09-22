#ifndef __ENGINE_GRAPHICSDEVICE_H__
#define __ENGINE_GRAPHICSDEVICE_H__

#include "glad/glad.h"

#include "viewport.hpp"
#include "window.hpp"
#include "color.hpp"

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