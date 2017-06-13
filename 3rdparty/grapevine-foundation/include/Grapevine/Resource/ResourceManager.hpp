/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_RESOURCE_RESOURCEMANAGER
#define GRAPEVINE_RESOURCE_RESOURCEMANAGER

#include "Grapevine/Core/Class.hpp"
#include <Grapevine/Core/Buffer.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Resource/MemoryFile.hpp>
#ifdef ANDROID
    #include <sys/types.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
#endif
#include <string>
#include <vector>

namespace Grapevine
{
    /**
     * @class   ResourceManager takes care that duplicate resource sharing
     *          To use Resource classes (mostly names as "Assets" ResourceManager
     *          instance has to be initialized.
     *
     * @note    ResourceManager must be existing usually the whole application lifetime.
     *
     * File class can be used to read files.
     */
    class ResourceManager
    {
    public:
        CLASS_UNCOPYABLE(ResourceManager)

        /**
         * ResourceManager instance has to be initialized.
         *
         * @param [in]  basePath       path to the resource files.
        */
        ResourceManager(char const* basePath = 0);
        virtual ~ResourceManager();
        #ifdef ANDROID
            static void setAssetManager(AAssetManager* assetManager);
            static void setAbsolutePath(const std::string& path);
            static std::string absolutePath();
            static FILE* android_fopen(const char* filename);
        #endif
        static std::vector<std::string> filesWithSuffix(
            std::string const& suffix,
            std::string const& path);

        /**
         * @param [in]  filename       filename (can be with relative path to assets/ folder)
         */
        static Buffer* loadFile(std::string const& filename);

        /**
         * @param [in]  filename       filename (can be with relative path to assets/ folder)
         */
        static ReferencedPointer<MemoryFile> mapFile(std::string const& filename);

    private:
        class ResourceManagerPrivate* private_;
    };
}

#endif
