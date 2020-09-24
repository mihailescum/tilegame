#include "core/resource.hpp"

namespace engine
{
    Resource::~Resource() { }

    void Resource::setResourceId(unsigned id) { this->resourceId = id; }
    unsigned Resource::getResourceId() const { return this->resourceId; }
    void Resource::setResourcePath(const std::string &path) { this->resourcePath = path; }
    std::string Resource::getResourcePath() const { return this->resourcePath; }
    void Resource::setResourceName(const std::string &name) { this->resourceName = name; }
    std::string Resource::getResourceName() const { return this->resourceName; }
} // namespace engine