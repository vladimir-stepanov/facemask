/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include "Grapevine/Resource/ResourceManager.hpp"
#include <string>

namespace Grapevine
{
    ResourceManager::ResourceManager(char const* path):
        private_(new ResourceManagerPrivate())
    {
        if (path != NULL)
        {
            ResourceManagerPrivate::setBasePath(std::string(path));
        }
    }

    ResourceManager::~ResourceManager()
    {
        delete private_;
    }

    #if defined(ANDROID) && !defined(__NGOS__)
        void ResourceManager::setAssetManager(AAssetManager* assetManager)
        {
            ResourceManagerPrivate::setAssetManager(assetManager);
        }

        void ResourceManager::setAbsolutePath(const std::string& path)
        {
            ResourceManagerPrivate::setAbsolutePath(path);
        }

        std::string ResourceManager::absolutePath()
        {
            return ResourceManagerPrivate::absolutePath();
        }
            
        FILE* ResourceManager::android_fopen(const char* filename)
        {
            return ResourceManagerPrivate::android_fopen(filename);
        }
    #endif

    std::vector<std::string> ResourceManager::filesWithSuffix(std::string const& suffix, std::string const& path)
    {
        return ResourceManagerPrivate::filesWithSuffix(suffix, path);
    }

    Buffer* ResourceManager::loadFile(std::string const& filename)
    {
        return ResourceManagerPrivate::loadFile(filename.c_str());
    }

    ReferencedPointer<MemoryFile> ResourceManager::mapFile(std::string const& filename)
    {
        return ResourceManagerPrivate::mapFile(filename.c_str());
    }
}
