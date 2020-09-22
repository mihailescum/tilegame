#include "core/log.hpp"

#include <iostream>

namespace engine
{
    void Log::d(std::string message)
    {
        std::cout << "[DEBUG] " << message << std::endl;
    }

    void Log::e(std::string message)
    {
        std::cout << "[ERROR] " << message << std::endl;
    }

    void Log::l(std::string message)
    {
        std::cout << "[LOG] " << message << std::endl;
    }
} // namespace engine