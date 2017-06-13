/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_RESOURCE_MEMORYFILE
#define GRAPEVINE_RESOURCE_MEMORYFILE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Resource/Resource.hpp>

#if defined(ANDROID) && !defined(__NGOS__)
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

#include <string>

namespace Grapevine
{
    /**
     * @class MemoryFile
     *
     * MemoryFile class can be used to read files.
     */
    class MemoryFile : public Resource
    {
    public:
        CLASS_UNCOPYABLE(MemoryFile)

        ~MemoryFile();
        //MemoryFile(MemoryFile &&file) = default;

        unsigned int byteSize() const;
        std::string const& name();

        template <typename T> operator T* () const
        {
            return reinterpret_cast<T*>(buffer_);
        }

        unsigned int read(char* targetBuffer, unsigned int amount);
        void seek(unsigned int amount);
        bool endOfFile();
        static bool hasEnding(std::string const &fullString, std::string const &ending);
        static bool exists(std::string const &filename);

        static std::string filenameFromPath(const std::string &fullPath)
        {
            unsigned found = fullPath.find_last_of("/\\");
            return fullPath.substr(found+1);
        }

    private:
        friend class ResourceManagerPrivate;

        MemoryFile(std::string const& filename, char* data, unsigned int dataSize);

        char* buffer_;
        unsigned int bufferSize_;
        char* bufferReadPointer_;
        unsigned int bufferBytesLeftToRead_;
        #if defined(ANDROID) && !defined(__NGOS__)
        AAsset* asset_ {nullptr};
        #endif
    };
}

#endif
