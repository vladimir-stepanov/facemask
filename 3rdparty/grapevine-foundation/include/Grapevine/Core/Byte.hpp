/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_BYTE
#define GRAPEVINE_CORE_BYTE

#include "Grapevine/Core/Log.hpp"
#include <stdint.h>

namespace Grapevine
{
    /**
     * @class Byte
     *
     * Byte a helper class for Byte level functions.
     */
    class Byte
    {
    public:
        UNCOPYABLE(Byte);

        static bool isSigned(unsigned char& byte)
        {
            return (0x01 << 7) & byte;
        }

        static int16_t int16(unsigned char* array)
        {
            return *(int*)array;
        }

    private:
        Byte() {};
        ~Byte() {};
    };
}

#endif
