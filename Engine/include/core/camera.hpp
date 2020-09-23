#ifndef __ENGINE_CAMERA_H__
#define __ENGINE_CAMERA_H__

#include "glm/glm.hpp"

#include "core/rectangle.hpp"
#include "core/viewport.hpp"

namespace engine
{
    class Camera
    {
        private:
            glm::mat4 transform;
            glm::vec2 position;
            Viewport viewport;

            void recomputeTransform();

        public:
            Camera(const Viewport &viewport);
            
            const glm::mat4 &getTransform() const;
            void setPosition(const glm::vec2 &position);
            void setViewport(const Viewport &viewport);
    };
} // namespace engine

#endif