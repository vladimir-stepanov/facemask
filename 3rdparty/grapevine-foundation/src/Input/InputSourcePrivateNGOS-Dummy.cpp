/**
 * Copyright (C) 2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Core/Log.hpp>
#include "InputSourcePrivateNGOS.hpp"

namespace Grapevine {

void InputSourcePrivateNGOS::PowerThread::run()
{
    LOGW("power button is not supported");
}

void InputSourcePrivateNGOS::VolumeThread::run()
{
    LOGW("volume button is not supported");
}

void InputSourcePrivateNGOS::TouchThread::run()
{
    LOGW("touch-screen is not supported");
}

} /* namespace Grapevine */
