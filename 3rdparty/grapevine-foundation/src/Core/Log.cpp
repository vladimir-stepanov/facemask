/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/Log.hpp"
#include <cstdio>
#include <cstdarg>
#include <string>
#include <sstream>
#ifndef __ANDROID__
    #include <unistd.h>
#endif

namespace Grapevine
{
    #ifdef __ANDROID__
        #include <android/log.h>
    #endif

    #ifndef __ANDROID__
        #if __APPLE__ && (TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE)
            static const char *terminalColorNormal ("");
            static const char *terminalColorRed ("");
            static const char *terminalColorBlue ("");
            static const char *terminalColorViolet ("");
            static const char *terminalColorBrown ("");
        #else
            static const char *terminalColorNormal ("\033[0m");
            static const char *terminalColorRed ("\033[1;31m");
            static const char *terminalColorBlue ("\033[34m");
            static const char *terminalColorViolet ("\033[35m");
            static const char *terminalColorBrown ("\033[33m");
        #endif

        static void prefixMessage (FILE *stream, const char *color,
            const char *prefix, const char *fmt, va_list ap)
        {
            va_list aq;

            // Estimate message size
            va_copy (aq, ap);
            int msg_size = vsnprintf(NULL, 0, fmt, aq);
            va_end (aq);

            // Buffer for the message
            char *buf = new char[msg_size + 1];

            // Store the message in the buffer
            va_copy (aq, ap);
            vsnprintf (buf, msg_size + 1, fmt, aq);
            va_end (aq);

            // Print the message lines prefixed with the supplied prefix.
            // If the target stream is a terminal make the prefix colored.
            bool useColor = isatty (fileno(stream));
            const char *startColor (useColor ? color : "");
            const char *endColor (useColor && *color ? terminalColorNormal : "");

            std::string line;
            std::stringstream ss(buf);

            while (std::getline(ss, line))
            {
                // If this line is a continuation of a previous log message
                // just print the line plainly.
                if (line[0] == LOG_CONTINUE[0])
                {
                    fprintf (stream, "%s", line.c_str() + 1);
                }
                else
                {
                    // Normal line, emit the prefix.
                    if (prefix != NULL)
                    {
                        fprintf (stream, "%s%s%s: %s", startColor, prefix, endColor,
                            line.c_str());
                    }
                    else
                    {
                        fprintf (stream, "%s", line.c_str());
                    }
                }

                if (!(ss.rdstate()))
                    fputs ("\n", stream);
            }
            fputs ("\n", stream);

            delete[] buf;
        }

        /**
         * Use this function for log prints that you want to stay in PR releases.
         */
        void Log::I(const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorBlue, "INFO", fmt, ap);
            va_end (ap);
        }

        /**
         * Use this function for log prints when something exeptional happens that
         * does not probably mean any near by crash or similar :) PR releases are
         * OK too with this.
         */
        void Log::W(const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorViolet, "WARNING", fmt, ap);
            va_end (ap);
        }

        /**
         * Use this function for log prints that are ment to be taken away from PR
         * release.
         */
        void Log::D(const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorBrown, "DEBUG", fmt, ap);
            va_end (ap);
        }

        /**
         * Use this function for log prints in error cases that mean highly propable
         * exiting the application.
         */
        void Log::E(const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stderr, terminalColorRed, "ERROR", fmt, ap);
            va_end (ap);
        }

        /**
         * Operator overloaded versions to support tagging
         */
        void Log::I(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorBlue, "INFO", fmt, ap);
            va_end (ap);
        }

        void Log::W(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorViolet, "WARNING", fmt, ap);
            va_end (ap);
        }

        void Log::D(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorBrown, "DEBUG", fmt, ap);
            va_end (ap);
        }

        void Log::E(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stderr, terminalColorRed, "ERROR", fmt, ap);
            va_end (ap);
        }

        void Log::LineChange()
        {
            fputs ("\n", stdout);
        }

        void Log::Flush()
        {
            fflush(stdout);
            fflush(stderr);
        }

        void Log::print(const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            prefixMessage (stdout, terminalColorBlue, 0, fmt, ap);
            va_end (ap);
        }
    #endif
    #ifdef __ANDROID__
        void Log::I(const char *fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            __android_log_vprint(ANDROID_LOG_INFO, AOS_LOG_TAG, fmt, ap);
            va_end(ap);
        }

        void Log::D(const char *fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            __android_log_vprint(ANDROID_LOG_DEBUG, AOS_LOG_TAG, fmt, ap);
            va_end(ap);
        }

        void Log::W(const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            __android_log_vprint(ANDROID_LOG_WARN, AOS_LOG_TAG, fmt, ap);
            va_end (ap);
        }

        void Log::E(const char *fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            __android_log_vprint(ANDROID_LOG_ERROR, AOS_LOG_TAG, fmt, ap);
            va_end(ap);
        }

        void Log::I(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            __android_log_vprint(ANDROID_LOG_INFO, tag->c_str(), fmt, ap);
            va_end(ap);
        }

        void Log::D(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            __android_log_vprint(ANDROID_LOG_DEBUG, tag->c_str(), fmt, ap);
            va_end(ap);
        }

        void Log::W(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start (ap, fmt);
            __android_log_vprint(ANDROID_LOG_WARN, tag->c_str(), fmt, ap);
            va_end (ap);
        }

        void Log::E(const std::string *tag, const char *fmt, ...)
        {
            va_list ap;
            va_start(ap, fmt);
            __android_log_vprint(ANDROID_LOG_ERROR, tag->c_str(), fmt, ap);
            va_end(ap);
        }

        void Log::Flush()
        {
        }
    #endif
}
