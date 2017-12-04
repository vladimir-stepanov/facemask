/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

/**
 * NOTE: This file is meant to be included _only_ if when creating new Resources classes.
 */

#ifndef GRAPEVINE_RESOURCE_RESOURCEMANAGERPRIVATE
#define GRAPEVINE_RESOURCE_RESOURCEMANAGERPRIVATE

#include "Grapevine/Resource/Resource.hpp"
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/Mutex.hpp>
#include <map>
#include <string>
#include <sigc++/connection.h>
#ifdef ANDROID
    #include <sys/types.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
#endif
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Core/Buffer.hpp>
#include <Grapevine/Resource/MemoryFile.hpp>
#include <vector>

static int android_read(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
    return -1; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}

namespace Grapevine
{
    class ResourceManagerPrivate: public Referenced
    {
    public:
        static void setBasePath(std::string const& path);
        static std::string basePath();
        static bool exists(char const* filename);

        ~ResourceManagerPrivate();
        static Resource* resource(std::string const& id);
        static void setResource(std::string const& id, Resource* resource);
        #ifdef ANDROID
            static void setAssetManager(AAssetManager* assetManager);
            static void setAbsolutePath(const std::string& path);
            static std::string absolutePath();
            static FILE* android_fopen(const char* filename) {
                AAsset* asset = AAssetManager_open(assetManager_, filename, 0);
                if(!asset) return NULL;
                return funopen(asset, android_read, android_write, android_seek, android_close);
            }
        #endif
        static Buffer* loadFile(char const* filename);
        static ReferencedPointer<MemoryFile> mapFile(char const* filename);
        static std::vector<std::string> filesWithSuffix(std::string const& suffix, std::string const& path = std::string(""));


    private:
        friend class ResourceManager;

        static std::string basePath_;
        static ResourceManagerPrivate* singleton_;

        ResourceManagerPrivate();
        void handleResourceDestroy(unsigned int);
        std::map<std::string, Resource*> resources_;
        std::map<unsigned int, sigc::connection> resourcesConnections_;
        Mutex mutex_;
        #ifdef ANDROID
            static AAssetManager* assetManager_;
            static std::string absolutePath_;
        #endif
    };
}

#endif
