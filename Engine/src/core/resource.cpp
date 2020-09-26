#include "core/resource.hpp"

namespace engine
{
    Resource::~Resource() { }

    void Resource::setResourceId(unsigned id) { this->resourceId = id; }
    unsigned Resource::getResourceId() const { return this->resourceId; }
    void Resource::setResourcePath(const std::filesystem::path &path) { this->resourcePath = path; }
    std::filesystem::path Resource::getResourcePath() const { return this->resourcePath; }
    void Resource::setResourceName(const std::string &name) { this->resourceName = name; }
    std::string Resource::getResourceName() const { return this->resourceName; }
} // namespace engine