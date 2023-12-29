#ifndef __ENGINE__GLERROR_HPP__
#define __ENGINE__GLERROR_HPP__

#include <iostream>

#include "glad/glad.h"

namespace engine
{
    GLenum glCheckError_(const char *file, int line);
}

#define glCheckError() engine::glCheckError_(__FILE__, __LINE__)

#endif