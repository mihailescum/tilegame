#include "core/resource.hpp"

namespace engine
{
    Resource::~Resource() { }

    void Resource::setResourceId(unsigned id) { this->resourceId = id; }
    unsigned Resource::getResourceId() const { return this->resourceId; }
    void Resource::setResourcePath(const std::string &path) { this->resourcePath = path; }
    std::string Resource::getResourcePath() const { return this->getResourcePath(); }
} // namespace engine