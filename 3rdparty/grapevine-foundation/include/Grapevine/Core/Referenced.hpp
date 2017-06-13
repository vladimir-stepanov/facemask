/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_REFERENCED_HPP
#define GRAPEVINE_CORE_REFERENCED_HPP

#include "Grapevine/Core/Class.hpp"
#include "Grapevine/Core/Log.hpp"
#include <cstddef>
using std::ptrdiff_t; // NOTE: sigc++ has problem without this line!
#include <sigc++/signal.h>
#include <atomic>

namespace Grapevine
{
#if defined(ANDROID_ATOMIC_FIX)
template< class T >
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
atomic_fetchadd( std::atomic<T>* obj, T arg )
{
    return obj->fetch_add(arg);
}

template< class T >
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type
atomic_fetchsub( std::atomic<T>* obj, T arg )
{
    return obj->fetch_sub(arg);
}
#endif

    /**
     * @class Referenced
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * Referenced class can be used to create reference counted classes.
     */
    class Referenced
    {
    public:
        CLASS_UNCOPYABLE(Referenced);

        sigc::signal<void, int> destroy;

        unsigned int increment()
        {
#if defined(ANDROID_ATOMIC_FIX)
            unsigned int r = Grapevine::atomic_fetchadd(&count_, 1u) + 1;
#else
            unsigned int r = std::atomic_fetch_add(&count_, 1u) + 1;
#endif
            return r;
        }

        unsigned int decrement()
        {
#if defined(ANDROID_ATOMIC_FIX)
            unsigned int r = Grapevine::atomic_fetchsub(&count_, 1u) - 1;
#else
            unsigned int r = std::atomic_fetch_sub(&count_, 1u) - 1;
#endif
            if (r <= 0) {
                destroy.emit(id_);
                delete this;
            }
            return r;
        }

        bool operator == (Referenced const& r) const
        {
            return id_ == r.id_;
        }

        unsigned int referenceCount() const
        {
            return count_;
        }

        unsigned int id() const
        {
            return id_;
        }

    protected:
        Referenced():
            count_(ATOMIC_VAR_INIT(0)),
#if defined(ANDROID_ATOMIC_FIX)
            id_(Grapevine::atomic_fetchadd(&lastId_, 1u))
#else
            id_(std::atomic_fetch_add(&lastId_, 1u))
#endif
        {
        }

        virtual ~Referenced()
        {
        }

    private:
        std::atomic<unsigned int> count_;
        unsigned int id_;
        //static unsigned int lastId_;
        static std::atomic<unsigned int> lastId_;
    };
}

#endif
