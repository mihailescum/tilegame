#pragma once

#include <string>
#include <iostream>

namespace engine
{
    /**
     * @brief Static console logging utility with severity-tagged output (debug/warning/error/log).
     *
     * Not instantiable; each level method accepts an arbitrary list of
     * streamable arguments, printed space-separated followed by a newline.
     */
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
