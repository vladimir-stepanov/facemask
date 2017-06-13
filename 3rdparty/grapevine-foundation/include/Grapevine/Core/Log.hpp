/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_LOG_HPP
#define GRAPEVINE_CORE_LOG_HPP

// Continuation of a previous log message on the same line.
#define LOG_CONTINUE "\x10"

#include <cstdio>
#include <string>

#ifndef AOS_LOG_TAG
#define AOS_LOG_TAG "Grapevine"
#endif

static const std::string sAosLogTag(AOS_LOG_TAG);

#if defined(DEBUG) || defined(__APPLE__)
#define LOG Grapevine::Log::D(&sAosLogTag, "(%s:%s:%d)", __FILE__, __FUNCTION__, __LINE__);
#define LOGD(...) Grapevine::Log::D(&sAosLogTag, __VA_ARGS__);
#else
#define LOG do {} while(0);
#define LOGD(...) do {} while(0);
#endif
#define LOGI(...) Grapevine::Log::I(&sAosLogTag, __VA_ARGS__);
#define LOGW(...) Grapevine::Log::W(&sAosLogTag, __VA_ARGS__);
#define LOGE(...) Grapevine::Log::E(&sAosLogTag, __VA_ARGS__);
#define LOGF Grapevine::LogFunction logFunction(__FILE__, __FUNCTION__, __LINE__);

namespace Grapevine
{
    /**
     * @class Log
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * Log a helper class to ease up debug, info and error logging.
     */
    class Log
    {
    public:
        static void I(const char *fmt, ...);
        static void W(const char *fmt, ...);
        static void D(const char *fmt, ...);
        static void E(const char *fmt, ...);

        static void I(const std::string *tag, const char *fmt, ...);
        static void W(const std::string *tag, const char *fmt, ...);
        static void D(const std::string *tag, const char *fmt, ...);
        static void E(const std::string *tag, const char *fmt, ...);

        static void LineChange();
        static void Flush();

        static void bit(char* data, int length)
        {
            unsigned char mask = 0x01;
            int ptr = 0;
            int bit = 0;

            for (; ptr < length; ptr++) {
                Log::print("Value %d in bits: [MSB ", (unsigned char)*(data+ptr));

                for (bit = 7; bit >= 0; bit--) {
                    if ((mask << bit) & (unsigned char)*(data+ptr)) {
                        Log::print("1");
                    } else {
                        Log::print("0");
                    }
                }
                Log::print(" LSB]\n");
            }
        }

        static void print(const char *fmt, ...);
    };

    class LogFunction
    {
    public:
        LogFunction(char const* file, char const* function, unsigned int line):
            file_(file),
            function_(function),
            line_(line)
        {
            LOGD("(%s:%s:%d)", file_, function_, line_);
        }

        ~LogFunction()
        {
            LOGD("(%s:%s:%d)", file_, function_, line_);
        }
    private:
        char const* file_;
        char const* function_;
        unsigned int line_;
    };
}

#endif
