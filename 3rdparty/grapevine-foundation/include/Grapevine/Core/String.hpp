/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_STRING_HPP
#define GRAPEVINE_CORE_STRING_HPP

#include <string>
#include <cstring>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Class.hpp>
#include <istream>
#include <sstream>
#include <vector>

namespace Grapevine
{
    /**
     * @class String
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * String is a helper class to provide extra functionality related
     * related to std::string.
     */
    class String
    {
    public:
        CLASS_UNCOPYABLE(String)

        static std::string toString(unsigned int number)
        {
            std::string r;

            if (number == 0)
            {
                r = std::string("0");
            }
            else
            {
                const char* table = "0123456789";

                // Count length
                unsigned int length = 0;
                unsigned int tmp = number;
                while (tmp > 0)
                {
                    tmp /= 10;
                    length++;
                }

                // Create string
                char* buffer = new char[length + 1];
                int index = length - 1;
                for (int i = index; i >= 0; i--)
                {
                    buffer[i] = table[number % 10];
                    number /= 10;
                }
                buffer[length] = '\0';
                r = std::string(buffer);
                delete [] buffer;
            }

            return r;
        }

        static std::string toString(int number)
        {
            std::string r;

            bool negative = (number < 0);
            if (negative)
            {
                number = -number;
            }

            r = toString((unsigned int)number);

            if (negative)
            {
                r = std::string("-") + r;
            }

            return r;
        }

        static std::string fromNonNullEnding(unsigned int length, char* data)
        {
            // Create tmp buffer
            char* tmpBuffer = new char[length + 1];
            memcpy(tmpBuffer, data, length);
            tmpBuffer[length] = '\0';

            // Create string from tmp buffer
            std::string r(tmpBuffer);

            // Delete tmp buffer
            delete [] tmpBuffer;

            return r;
        }

        static void split(const std::string &s, char delimiter, std::vector<std::string> &elems)
        {
            std::stringstream ss(s);
            std::string item;
            while(std::getline(ss, item, delimiter)) {
                elems.push_back(item);
            }
        }

    private:
        String() {}
        ~String() {}
        char* string_;
    };
}

#endif
