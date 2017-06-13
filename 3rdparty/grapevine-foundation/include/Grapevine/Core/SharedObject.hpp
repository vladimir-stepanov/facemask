/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_CORE_SHAREDOBJECT
#define GRAPEVINE_CORE_SHAREDOBJECT

#include "Grapevine/Core/Shared.hpp"
#include "Grapevine/Core/Class.hpp"
#include <cstddef>
using std::ptrdiff_t; // NOTE: sigc++ has problem without this line!
#include <sigc++/signal.h>
#include <typeinfo>

namespace Grapevine
{
    /**
     * @class SharedObject
     *
     * SharedObject class should be used with Shared as the base class
     * for the managed object.
     */
    class SharedObject
    {
    public:
        CLASS_UNCOPYABLE(SharedObject);

        sigc::signal<void, int> destroy;

        bool operator == (SharedObject const& r) const
        {
            return id_ == r.id_;
        }

        unsigned int id() const
        {
            return id_;
        }

        virtual const std::type_info &typeInfo() const
        {
            return typeid(SharedObject);
        }

        SharedObject():
            id_(lastId_++)
        {
        }

        virtual ~SharedObject()
        {
            destroy.emit(id_);
        }

    private:
        unsigned int id_;
        static unsigned int lastId_;
    };

}

#endif
