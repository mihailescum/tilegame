#include "core/resourcemanager.hpp"

#include <fstream>
#include <sstream>
#include <cstdarg>

namespace engine
{
    ResourceManager::~ResourceManager()
    {
        this->unloadResources();
    }

    // properly de-allocates all loaded resources
    void ResourceManager::unloadResources()
    {
        for (auto &iter : this->resources)
            iter.second->unloadResource();
    }

    Resource *ResourceManager::getResource(const std::string &name) const
    {
        if (this->resources.count(name) != 0)
            return this->resources.at(name).get();
        else
            return nullptr;
    }
} // namespace engine