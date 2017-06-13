/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_UTIL
#define GRAPEVINE_CORE_UTIL

#include "Grapevine/Core/Class.hpp"
#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <stdint.h>

namespace Grapevine
{
    /**
     * @class Util
     *
     * Util is a general helper class.
     */
    struct Util
    {
        /**
         * NOTE: please use the String::split() implementation instead. To be deprecated!
         */
        static void split(const std::string &s, char delimiter, std::vector<std::string> &elems)
        {
            std::stringstream ss(s);
            std::string item;
            while(std::getline(ss, item, delimiter))
            {
                elems.push_back(item);
            }
        }

        template<typename T> static T fromString(const std::string& asString)
        {
            std::stringstream ss(asString);
            T r;
            ss >> r;
            return r;
        }

        static inline float clamp(float x, float min, float max)
        {
            return x < min ? min : (x > max ? max : x);
        }
    };
}

#endif
