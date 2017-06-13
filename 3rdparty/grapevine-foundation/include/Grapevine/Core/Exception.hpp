/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_EXCEPTION_HPP
#define GRAPEVINE_CORE_EXCEPTION_HPP

#include "Grapevine/Core/String.hpp"
#ifdef __APPLE__
#include <exception>
#else
#include <stdexcept>
#endif
#include <string>
#include "Grapevine/Core/Class.hpp"

namespace Grapevine
{
    /**
     * @class Exception
     *
     * Exception is Grapevine's own Exception class thrown in fatal
     * error cases where application state is corrupted.
     */
    class Exception: public std::exception
    {
    public:
        // CLASS_COPYABLE(Exception);

        explicit Exception(char const* message): std::exception(),
            message_(message)
        {
        }

        virtual ~Exception() throw()
        {

        }

        virtual void raise()
        {
            throw *this;
        }

        virtual char const* what() const throw()
        {
            return message_;
        }

        Exception(Exception const& other): message_(other.message_)
        {
        }

    private:
        char const* message_;
    };
}

#endif
