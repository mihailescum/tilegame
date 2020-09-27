#pragma once

#include <string>

namespace engine
{
    class Scriptable
    {
    private:
        std::string scriptId;

    protected:
        Scriptable(const std::string &id) : scriptId(id) {};

    public:
        void setScriptId(const std::string &id);
        const std::string &getScriptId() const;
    };
} // namespace engine