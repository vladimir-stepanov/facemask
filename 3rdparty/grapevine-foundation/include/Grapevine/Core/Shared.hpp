/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_SHARED
#define GRAPEVINE_CORE_SHARED

#include <memory>

namespace Grapevine
{
    /**
     * @class Shared
     *
     * Shared class can be used to create reference counted classes.
     */
    template <typename T>
    class Shared : public std::shared_ptr<T>
    {
    public:

        template <typename ...A>
        static Shared<T> make(A&&... a)
        {
            return new T(std::forward<A>(a)...);
        }

        Shared(T *ptr)
        : std::shared_ptr<T>(ptr)
        {
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        template <typename U>
        Shared(const Shared<U> &shared)
        : std::shared_ptr<T>(std::static_pointer_cast<T>(shared))
        {
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        Shared(const Shared& shared)
        {
            *this = shared;
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        ~Shared()
        {
            //printf("  ~Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
        }

        const Shared& operator = (const Shared& shared)
        {
            std::shared_ptr<T>::operator = (shared);
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
            return *this;
        }

        template <typename U>
        const Shared& operator = (const Shared<U>& shared)
        {
            std::shared_ptr<T>::operator = (std::static_pointer_cast<T>(shared));
            //printf("  Shared count: %d (%p)\n", count(), std::shared_ptr<T>::get());
            return *this;
        }

        unsigned int count() const
        {
            return int(this->use_count());
        }

        unsigned int id() const
        {
            return int(this->id());
        }

        bool isNull() const
        {
            return !std::shared_ptr<T>::get();
        }
    };
}

#endif
