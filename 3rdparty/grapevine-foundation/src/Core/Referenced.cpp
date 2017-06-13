/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Core/Referenced.hpp"

namespace Grapevine
{
    std::atomic<unsigned int> Referenced::lastId_ = ATOMIC_VAR_INIT(0);
}
