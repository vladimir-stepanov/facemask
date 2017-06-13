/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include <Grapevine/Resource/MemoryFile.hpp>
#include <Grapevine/Core/Log.hpp>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <sys/mman.h> //munmap
#include <errno.h> //errno

#ifdef ANDROID
    #include <sys/types.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
#endif

namespace Grapevine
{
    MemoryFile::~MemoryFile()
    {
        if (buffer_)
        {
            if (munmap(buffer_, bufferSize_) != 0)
            {
                int error = errno;
                LOGE("munmap failed %d", error);
            }

            buffer_ = nullptr;
            bufferSize_ = 0;
            bufferReadPointer_ = nullptr;
            bufferBytesLeftToRead_ = 0;
        }
        #if defined(ANDROID) && !defined(__NGOS__)
        if (asset_)
        {
            AAsset_close(asset_);
            asset_ = nullptr;
        }
        #endif
    }

    MemoryFile::MemoryFile(std::string const& filename, char* data, unsigned int dataSize):
        Resource(filename),
        buffer_(data),
        bufferSize_(dataSize),
        bufferReadPointer_(data),
        bufferBytesLeftToRead_(dataSize)
    {
    }

    unsigned int MemoryFile::byteSize() const
    {
        return bufferSize_;
    }

    std::string const& MemoryFile::name()
    {
        return path();
    }

    bool MemoryFile::hasEnding(std::string const &fullString, std::string const &ending)
    {
        if (fullString.length() >= ending.length())
        {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        }
        else
        {
            return false;
        }
    }

    unsigned int MemoryFile::read(char* targetBuffer, unsigned int byteAmount)
    {
        unsigned int r = 0;
        if (bufferBytesLeftToRead_ < byteAmount)
        {
            r = bufferBytesLeftToRead_;
            bufferBytesLeftToRead_ = 0;
        }
        else
        {
            r = byteAmount;
            bufferBytesLeftToRead_ -= byteAmount;
        }
        memcpy(targetBuffer, bufferReadPointer_, r);
        bufferReadPointer_ += r;
        return r;
    }

    void MemoryFile::seek(unsigned int amount)
    {
        if (bufferBytesLeftToRead_ < amount)
        {
            bufferReadPointer_ += bufferBytesLeftToRead_;
            bufferBytesLeftToRead_ = 0;
        }
        else
        {
            bufferReadPointer_ += amount;
            bufferBytesLeftToRead_ -= amount;
        }
    }

    bool MemoryFile::endOfFile()
    {
        return (bufferBytesLeftToRead_ == 0) ? true : false;
    }

    bool MemoryFile::exists(std::string const& filename)
    {
        return ResourceManagerPrivate::exists(filename.c_str());
    }
}
