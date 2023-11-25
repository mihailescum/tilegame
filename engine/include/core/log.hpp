#pragma once

#include <string>
#include <iostream>

namespace engine
{
    class Log
    {
    private:
        Log();

        template <typename Arg, typename... Args>
        static void print(Arg &&arg, Args &&...args)
        {
            std::cout << std::forward<Arg>(arg);
            using expander = int[];
            (void)expander{0, (void(std::cout << std::forward<Args>(args) << " "), 0)...};
            std::cout << std::endl;
        }

    public:
        template <typename Arg, typename... Args>
        static void d(Arg &&arg, Args &&...args)
        {
            Log::print("[DEBUG] ", arg, args...);
        }

        template <typename Arg, typename... Args>
        static void w(Arg &&arg, Args &&...args)
        {
            Log::print("[WARNING] ", arg, args...);
        }

        template <typename Arg, typename... Args>
        static void e(Arg &&arg, Args &&...args)
        {
            Log::print("[ERROR] ", arg, args...);
        }

        template <typename Arg, typename... Args>
        static void l(Arg &&arg, Args &&...args)
        {
            Log::print("[LOG] ", arg, args...);
        }
    };
} // namespace engine
