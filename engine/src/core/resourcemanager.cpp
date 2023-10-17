#include "core/resourcemanager.hpp"

#include <fstream>
#include <sstream>
#include <cstdarg>

namespace engine
{
    ResourceManager::~ResourceManager()
    {
        unloadResources();
    }

    // properly de-allocates all loaded resources
    void ResourceManager::unloadResources()
    {
        for (auto &iter : resources)
            iter.second->unloadResource();
    }

    Resource *ResourceManager::getResource(const std::string &name) const
    {
        if (resources.count(name) != 0)
            return resources.at(name).get();
        else
            return nullptr;
    }
} // namespace engine