#ifndef __ENGINE__GLERROR_HPP__
#define __ENGINE__GLERROR_HPP__

#include <iostream>

#include "glad/glad.h"

namespace engine
{
    /// Drains and logs all pending OpenGL errors (via glGetError) to stdout, tagged with the calling file/line; returns the last error code encountered.
    GLenum glCheckError_(const char *file, int line);
}

/// Convenience macro that calls glCheckError_() with the current file and line.
#define glCheckError() engine::glCheckError_(__FILE__, __LINE__)

#endif