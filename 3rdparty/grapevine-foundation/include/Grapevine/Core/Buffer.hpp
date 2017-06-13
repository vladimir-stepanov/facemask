/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_BUFFER
#define GRAPEVINE_CORE_BUFFER

#include "Grapevine/Core/Referenced.hpp"
#include "Grapevine/Core/Exception.hpp"
#include "Grapevine/Core/Class.hpp"
#include <string.h>

namespace Grapevine
{
    /**
     * @class Buffer
     *
     * Buffer is a class that can store referenced blocks of data.
     */
    class Buffer: public Referenced
    {
    public:
        CLASS_UNCOPYABLE(Buffer)

        Buffer(char* data, unsigned int byteSize, bool manageDataLifetime = true):
            Referenced(),
            data_(data),
            byteSize_(byteSize),
            manageDataLifetime_(manageDataLifetime)
        {
            if (byteSize > 0 && data == NULL)
            {
                throw Exception("Buffer has to have data provided if it has byteSize > 0");
            }
            else if (byteSize == 0 && data != NULL)
            {
                throw Exception("Buffer should not have data provided if it has byteSize == 0");
            }
        }

        ~Buffer()
        {
            if (manageDataLifetime_ && data_ != NULL)
            {
                delete [] data_;
                data_ = NULL;
            }
        }

        char* data()
        {
            return data_;
        }

        unsigned int byteSize()
        {
            return byteSize_;
        }

        // TODO: fix the dataReadPointer to read index or similar.
        void read(char* targetBuffer, char* dataReadPointer, unsigned int byteAmount)
        {
            memcpy(targetBuffer, dataReadPointer, byteAmount);
        }

    private:
        char* data_;
        unsigned int byteSize_;
        bool manageDataLifetime_;
    };
}

#endif
