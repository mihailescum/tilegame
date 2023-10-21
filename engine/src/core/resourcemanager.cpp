#include "core/resourcemanager.hpp"

#include <fstream>
#include <sstream>
#include <cstdarg>

namespace engine
{
    ResourceManager::~ResourceManager()
    {
        unload_resources();
    }

    // properly de-allocates all loaded resources
    void ResourceManager::unload_resources()
    {
        for (auto &iter : _resources)
            iter.second->unload_resource();
    }

    Resource *ResourceManager::get_resource(const std::string &name) const
    {
        if (_resources.count(name) != 0)
            return _resources.at(name).get();
        else
            return nullptr;
    }
} // namespace engine