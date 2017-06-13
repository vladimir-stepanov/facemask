/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_RANDOM_HPP
#define GRAPEVINE_CORE_RANDOM_HPP

#include "Grapevine/Core/Class.hpp"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

namespace Grapevine
{
    /**
     * @class Random
     *
     * @note    For more serious than simple cases the random numbers
     *          provided by this class is not good enough!
     *
     * Random class is a helper class used to get random numbers.
     */
    class Random
    {
    public:
        CLASS_UNCOPYABLE(Random);

        static float zeroToOneFloat()
        {
            init();
            return ((float)rand()) / ((float)RAND_MAX);
        }

        static unsigned int integerWithMod(unsigned int mod)
        {
            init();
            return rand() % mod;
        }

    private:
        Random() {};
        ~Random() {};

        static void init()
        {
            static bool initDone = false;

            if (initDone == false)
            {
                srand(time(NULL));
                initDone = true;
            }
        }
        static bool initDone_;
    };
}

#endif
