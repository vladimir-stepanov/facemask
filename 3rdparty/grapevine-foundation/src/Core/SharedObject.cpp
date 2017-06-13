/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/SharedObject.hpp"

namespace Grapevine
{
    class SharedNull : public SharedObject
    {
    public:
        SharedNull() {}
        virtual ~SharedNull() {}
    };

    unsigned int SharedObject::lastId_ = 0;
}
