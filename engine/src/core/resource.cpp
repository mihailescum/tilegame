#include "core/resource.hpp"

namespace engine
{
    Resource::~Resource() {}

    void Resource::resource_id(std::size_t id) { _resource_id = id; }
    std::size_t Resource::resource_id() const { return _resource_id; }
    void Resource::resource_path(const std::filesystem::path &path) { _resource_path = path; }
    std::filesystem::path Resource::resource_path() const { return _resource_path; }
    void Resource::resource_name(const std::string &name) { _resource_name = name; }
    std::string Resource::resource_name() const { return _resource_name; }
} // namespace engine