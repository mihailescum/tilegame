#ifndef __ENGINE_LOG_H__
#define __ENGINE_LOG_H__

#include <string>

namespace engine
{
    class Log
    {
    public:
        static void d(std::string message);
        static void e(std::string message);
        static void l(std::string message);

    private:
        Log();
    };
} // namespace engine

#endif