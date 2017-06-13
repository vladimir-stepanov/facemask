/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_RESOURCE_FILE
#define GRAPEVINE_RESOURCE_FILE

#include <Grapevine/Core/Buffer.hpp>
#include <Grapevine/Core/Class.hpp>
#include <string>

namespace Grapevine
{
    /**
     * @class File
     *
     * File class can be used to read files.
     */
    class File
    {
    public:
        CLASS_UNCOPYABLE(File)

        File(std::string const& filename);
        ~File();
        unsigned int byteSize() const;
        std::string const& name();

        template <typename T> operator T* () const
        {
            return reinterpret_cast<T*>(buffer_->data());
        }

        unsigned int read(char* targetBuffer, unsigned int amount);
        void seek(unsigned int amount);
        bool endOfFile();
        static bool hasEnding(std::string const &fullString, std::string const &ending);
        static bool exists(std::string const& filename);

        static std::string filenameFromPath(const std::string& fullPath)
        {
            unsigned found = fullPath.find_last_of("/\\");
            return fullPath.substr(found+1);
        }

    private:
        std::string filename_;
        Buffer* buffer_;
        char* bufferReadPointer_;
        unsigned int bufferBytesLeftToRead_;
    };
}

#endif
