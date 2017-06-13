/**
 * Copyright (C) 2013-2017 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Resource/ResourceManagerPrivate.hpp>
#include <Grapevine/Resource/File.hpp>
#include <Grapevine/Core/Log.hpp>
#include <cstdio>
#include <cstddef>
#include <cstdlib>

namespace Grapevine
{
    File::File(std::string const& filename):
        filename_(filename),
        buffer_(ResourceManagerPrivate::loadFile(filename.c_str())),
        bufferReadPointer_(NULL),
        bufferBytesLeftToRead_(0)
    {
        // TODO: handle same buffers with reference
        buffer_->increment();
        bufferBytesLeftToRead_ = buffer_->byteSize();
        bufferReadPointer_ = buffer_->data();
        //LOGD("File, bytes to read:%d", bufferBytesLeftToRead_);
    }

    File::~File()
    {
        buffer_->decrement();
        buffer_ = NULL;
    }

    unsigned int File::byteSize() const
    {
        return buffer_->byteSize();
    }

    std::string const& File::name()
    {
        return filename_;
    }

    bool File::hasEnding(std::string const &fullString, std::string const &ending)
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

    unsigned int File::read(char* targetBuffer, unsigned int byteAmount)
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
        buffer_->read(targetBuffer, bufferReadPointer_, r);
        bufferReadPointer_ += r;
        return r;
    }

    void File::seek(unsigned int amount)
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

    bool File::endOfFile()
    {
        return (bufferBytesLeftToRead_ == 0) ? true : false;
    }

    bool File::exists(std::string const& filename)
    {
        return ResourceManagerPrivate::exists(filename.c_str());
    }
}
