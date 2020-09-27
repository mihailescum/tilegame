#pragma once

#include <string>

namespace engine
{
    class Log
    {
    private:
        Log();

    public:
        static void d(std::string message);
        static void e(std::string message);
        static void l(std::string message);
    };
} // namespace engine
