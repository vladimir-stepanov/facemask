/**
 * Copyright (C) 2013-2015 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_REFERENCEDPOINTER_HPP
#define GRAPEVINE_CORE_REFERENCEDPOINTER_HPP

#include <stddef.h>
#include "Grapevine/Core/Class.hpp"
#include "Grapevine/Core/Referenced.hpp"
#include "Grapevine/Core/Exception.hpp"

namespace Grapevine
{
    /**
     * @class ReferencedPointer
     *
     * NOTE: please use the .h version as your include in Atelier OS project! This header will be deprecated!
     *
     * ReferencedPointer class can be used to hold reference to
     * classes that are inherited from Referenced.
     */
    template <class T> class ReferencedPointer
    {
    public:
        // CLASS_COPYABLE

        ReferencedPointer():
            instance_(NULL)
        {
        }

        ReferencedPointer(T* instance):
            instance_(instance)
        {
            if (instance_ != NULL) {
                Referenced* sanityCheck = dynamic_cast<Referenced*>(instance);
                if (sanityCheck == NULL) {
                    throw Exception("ReferencedPointer can hold only Referenced classes.");
                }
                instance_->increment();
            }
        }

        ~ReferencedPointer()
        {
            if (instance_ != NULL) {
                instance_->decrement();
                instance_ = NULL;
            }
        }

        ReferencedPointer& operator=(ReferencedPointer const& r)
        {
            if (this != &r) {
                if (instance_ != NULL) {
                    instance_->decrement();
                    instance_ = NULL;
                }

                if (r.instance_ != NULL) {
                    instance_ = r.instance_;
                    instance_->increment();
                }
            }
            return *this;
        }

        ReferencedPointer(ReferencedPointer const& r)
        {
            instance_ = r.instance_;
            if (instance_ != NULL) {
                instance_->increment();
            }
        }

        bool isNull() const
        {
            if (instance_ == NULL) {
                return true;
            } else {
                return false;
            }
        }

        bool exists() const
        {
            if (instance_ != NULL) {
                return true;
            } else {
                return false;
            }
        }

        T* pointer() const
        {
            return instance_;
        }

        T* operator-> () const
        {
            return instance_;
        }

        T& operator* () const
        {
            return instance_;
        }

        bool operator == (ReferencedPointer const& r) const
        {
            if (instance_ != NULL && r.instance_ != NULL) {
                return *instance_ == *(r.instance_);
            }

            return false;
        }

    private:
        T* instance_;
    };
}

#endif
