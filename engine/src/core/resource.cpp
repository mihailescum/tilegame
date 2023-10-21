#include "core/resource.hpp"

namespace engine
{
    Resource::~Resource() {}

    void Resource::set_resource_id(std::size_t id) { this->_resource_id = id; }
    std::size_t Resource::get_resource_id() const { return this->_resource_id; }
    void Resource::set_resource_path(const std::filesystem::path &path) { this->_resource_path = path; }
    std::filesystem::path Resource::get_resource_path() const { return this->_resource_path; }
    void Resource::set_resource_name(const std::string &name) { this->_resource_name = name; }
    std::string Resource::get_resource_name() const { return this->_resource_name; }
} // namespace engine