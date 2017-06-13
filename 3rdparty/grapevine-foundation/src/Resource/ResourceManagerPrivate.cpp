/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include <Grapevine/Resource/File.hpp>
#include <Grapevine/Core/Lock.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <dirent.h> // filesWithSuffix()
#include <fcntl.h> //open
#include <errno.h> //errno
#include <sys/stat.h> //fstat
#include <sys/mman.h> //mmap
#include <unistd.h> // close

namespace Grapevine
{
    std::string ResourceManagerPrivate::basePath_ = std::string("");
    ResourceManagerPrivate* ResourceManagerPrivate::singleton_ = NULL;
    #if defined(ANDROID) && !defined(__NGOS__)
        AAssetManager* ResourceManagerPrivate::assetManager_ = NULL;
        std::string ResourceManagerPrivate::absolutePath_ = "";
    #endif

    ResourceManagerPrivate::ResourceManagerPrivate()
    {
        if (singleton_ != NULL)
        {
            LOGE("ResourceManagerPrivate (==ResourceManager in public API!) exists already.");
            throw Exception("ResourceManagerPrivate exists already.");
        }
        singleton_ = this;
    }

    ResourceManagerPrivate::~ResourceManagerPrivate()
    {
        if (singleton_->resources_.size() == 0)
        {
            LOGI("ResourceManager: all resources freed.");
        }
        else
        {
            LOGI("ResourceManager: resources having references:");
            std::map<std::string, Resource*>::iterator i;
            for (i = singleton_->resources_.begin(); i != singleton_->resources_.end(); i++)
            {
                std::string id = i->first;
                Resource* resource = i->second;
                LOGI(" -(id:%s)(refcount:%d)", id.c_str(), resource->referenceCount());
            }
        }
        singleton_ = NULL;
    }

    Resource* ResourceManagerPrivate::resource(std::string const& id)
    {
        if (singleton_ == NULL)
        {
            throw Exception("Resource creation possible only during ResourceManager instance's lifetime.");
        }

        Lock lock(singleton_->mutex_);

        Resource* r = NULL;
        std::string key(id);
        std::map<std::string, Resource*>::const_iterator i;
        i = singleton_->resources_.find(key);
        if (i != singleton_->resources_.end())
        {
            r = i->second;
        }
        return r;
    }

    void ResourceManagerPrivate::setResource(std::string const& id, Resource* resource)
    {
        Lock lock(singleton_->mutex_);

        singleton_->resources_.insert(std::make_pair(id, resource));

        // Listen for destroy signals. We don't hold reference count in ResourceManager.
        sigc::connection connection = resource->destroy.connect(
            sigc::mem_fun(singleton_, &ResourceManagerPrivate::handleResourceDestroy));

        singleton_->resourcesConnections_.insert(std::make_pair(resource->id(), connection));
    }

    void ResourceManagerPrivate::handleResourceDestroy(unsigned int id)
    {
        if(singleton_!=NULL){
            Lock lock(singleton_->mutex_);

            // Remove Resource from loaded resources list and signal listener list, TODO change to map::find()
            std::map<std::string, Resource*>::iterator i;
            for (i = singleton_->resources_.begin(); i != singleton_->resources_.end(); i++)
            {
                Resource* loadedResource = i->second;
                if (loadedResource->id() == id)
                {
                    // Remove Resource's destroy signal connection
                    std::map<unsigned int, sigc::connection>::iterator i2;
                    i2 = singleton_->resourcesConnections_.find(id);
                    singleton_->resourcesConnections_.erase(i2);

                    // Resource will delete itself, look Referenced.hpp
                    singleton_->resources_.erase(i);

                    break;
                }
            }
        }
    }

    #if defined(ANDROID) && !defined(__NGOS__)
        void ResourceManagerPrivate::setAssetManager(AAssetManager* assetManager)
        {
            /*if (assetManager_ != NULL)
            {
                LOGE("ResourceManagerPrivate(), AAssetManager already set!");
                throw Exception("ResourceManagerPrivate(), AAssetManager already set!");
            }*/

            assetManager_ = assetManager;
        }

        void ResourceManagerPrivate::setAbsolutePath(const std::string& path)
        {
            absolutePath_ = path;
        }

        std::string ResourceManagerPrivate::absolutePath()
        {
            return absolutePath_;
        }
    #endif

    Buffer* ResourceManagerPrivate::loadFile(char const* filename)
    {
        char* buffer = NULL;
        unsigned int bufferByteSize = 0;

        #if defined(ANDROID) && !defined(__NGOS__)
            AAsset* asset = AAssetManager_open(assetManager_, filename, AASSET_MODE_STREAMING);
            if (asset == NULL)
            {
                LOGE("No Android asset, failed to load filename: %s", filename);
                throw Exception("ResourceManagerPrivate::loadFile() failed");
            }
            bufferByteSize = AAsset_getLength(asset);

            buffer = new char[bufferByteSize];
            int nb_read = 0;
            while ((nb_read = AAsset_read(asset, buffer, bufferByteSize)) > 0)
            {
            }
            AAsset_close(asset);
        #else
            std::string fullName;
            fullName += basePath_;
            fullName += std::string(filename);

            FILE *file = fopen(fullName.c_str(), "rb");
            if (!file)
            {
                LOGE("No FILE, failed to load basename: %s", basePath_.c_str());
                LOGE("No FILE, failed to load filename: %s", filename);
                throw Exception("ResourceManagerPrivate::loadFile() failed");
            }

            fseek(file, 0, SEEK_END);
            bufferByteSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            buffer = new char[bufferByteSize];
            fread(buffer, 1, bufferByteSize, file);
            fclose(file);
        #endif

        return new Buffer(buffer, bufferByteSize);
    }

    ReferencedPointer<MemoryFile> ResourceManagerPrivate::mapFile(char const* filename)
    {
        char* buffer = nullptr;
        unsigned int bufferByteSize = 0;
        ReferencedPointer<MemoryFile> memoryfile;

        #if defined(ANDROID) && !defined(__NGOS__)
        #warning "verify AAssetManager mapping and cleanup the AAsset"

            AAsset* asset = AAssetManager_open(assetManager_, filename, AASSET_MODE_BUFFER);
            if (asset == NULL)
            {
                LOGE("No Android asset, failed to load filename: %s", filename);
                throw Exception("ResourceManagerPrivate::loadFile() failed");
            }
            bufferByteSize = AAsset_getLength(asset);

            buffer = (char*)AAsset_getBuffer(asset);

            memoryfile = ReferencedPointer<MemoryFile>(new MemoryFile(std::string(filename), buffer, bufferByteSize));
            memoryfile->asset_ = asset;
        #else
            std::string fullName {basePath_};
            fullName += std::string(filename);

            int fileDescriptor = open(fullName.c_str(), O_RDONLY);
            if (fileDescriptor == -1)
            {
                int error = errno;
                LOGE("No FILE (%d), failed to load basename: \"%s\" filename: \"%s\"", error, basePath_.c_str(), filename);
                throw Exception("ResourceManagerPrivate::mapFile() failed to open");
            }

            struct stat fileStatus;
            if (fstat(fileDescriptor, &fileStatus) == -1)
            {
                int error = errno;
                LOGE("failed to get file status %s, %d", fullName.c_str(), error);
                throw Exception("ResourceManagerPrivate::mapFile() failed to get status");
            }

            buffer = static_cast<char*>(mmap(nullptr, fileStatus.st_size,  PROT_READ, MAP_SHARED, fileDescriptor, 0));
            if (!buffer)
            {
                int error = errno;
                LOGE("failed to map file %s, %d", fullName.c_str(), error);
                close(fileDescriptor);
                throw Exception("ResourceManagerPrivate::mapFile() failed to map file");
            }
            else
            {
                bufferByteSize = fileStatus.st_size;
                LOGI(" mapped 0x%x <- %s, size %u", buffer, fullName.c_str(), bufferByteSize);
            }
            close(fileDescriptor);

            memoryfile = ReferencedPointer<MemoryFile>(new MemoryFile(std::string(filename), buffer, bufferByteSize));
        #endif

        return memoryfile;
    }

    std::vector<std::string> ResourceManagerPrivate::filesWithSuffix(std::string const& suffix, std::string const& pathString)
    {
        std::vector<std::string> r;

        #if defined(ANDROID) && !defined(__NGOS__)
            // Make sure the directory where we will be writing (logs, replays) exists.
            const char* path = pathString.c_str();
            const char* assetUrl;
            char* cursor;
            char directoryPath[1024];

            AAssetDir* directory = AAssetManager_openDir(assetManager_, path);

            strcpy(directoryPath, path);
            cursor = &directoryPath[strlen(directoryPath)-1];
            while (*(cursor-1) != '/')
            {
                cursor--;
            }
            cursor--;
            *cursor = '\0';

            while ((assetUrl = AAssetDir_getNextFileName(directory)) != NULL)
            {
                std::string tmpFile(assetUrl);
                if (File::hasEnding(tmpFile, suffix))
                {
                    r.push_back(pathString + std::string("/") + tmpFile);
                }
            }
            fflush(stdout);

            AAssetDir_close(directory);
        #else
            DIR* dpdf;
            struct dirent* epdf;

            std::string tmp = basePath_ + std::string("/") + pathString;
            dpdf = opendir(tmp.c_str());
            if (dpdf != NULL)
            {
                while ((epdf = readdir(dpdf)) != NULL)
                {
                    std::string tmpFile(epdf->d_name);
                    if (File::hasEnding(tmpFile, suffix))
                    {
                        r.push_back(pathString + std::string("/") + tmpFile);
                    }
                }

                closedir(dpdf);
            }
        #endif

        return r;
    }

    void ResourceManagerPrivate::setBasePath(std::string const& path)
    {
        basePath_ = path;
    }

    std::string ResourceManagerPrivate::basePath()
    {
        return basePath_;
    }

    bool ResourceManagerPrivate::exists(char const* filename)
    {
        bool r = false;

        #if defined(ANDROID) && !defined(__NGOS__)
            AAsset* asset = AAssetManager_open(assetManager_, filename, AASSET_MODE_STREAMING);
            if (asset != NULL)
            {
                r = true;
                AAsset_close(asset);
            }
        #else
            // TODO: no need to open the file, polish
            std::string fullName;
            fullName += basePath_;
            fullName += std::string(filename);

            FILE *file = fopen(fullName.c_str(), "rb");
            if (file)
            {
                r = true;
                fclose(file);
            }
        #endif
        return r;
    }
}
