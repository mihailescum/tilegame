#include "scripting/scriptable.cpp"

namespace engine
{
    void Scriptable::setScriptId(const std::string &id) { this->scriptId = id; }
    const std::string &Scriptable::getScriptId() const { return this->scriptId; }
} // namespace engine