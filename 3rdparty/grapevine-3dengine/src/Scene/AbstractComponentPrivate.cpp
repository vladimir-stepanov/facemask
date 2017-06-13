/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/AbstractComponentPrivate.hpp"
#include <Grapevine/Config.hpp>
#include <algorithm>

namespace Grapevine
{
    namespace Scene
    {
#ifdef GRAPEVINE_CONFIG_DEBUG
        unsigned int AbstractComponentPrivate::componentAmount_ = 0;
#endif
    }
}
