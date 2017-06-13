/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Input/AppleTouchTracker.hpp"
#include <Grapevine/Core/Log.hpp>

namespace Grapevine
{
    int AppleTouchTracker::getId(void* touch)
    {
        for (int i = 0; i < AppleTouchTracker::MaxTouches; i++)
        {
            if (touches_[i] == touch)
            {
                return i;
            }
        }

        return -1;
    }

    int AppleTouchTracker::addNewTouch(void* touch)
    {
        for (int i = 0; i < AppleTouchTracker::MaxTouches; i++)
        {
            if (touches_[i] == NULL)
            {
                // Put to empty slot
                touches_[i] = touch;
                return i;
            }
        }

        Log::E("Can't add new touch id");
        return -1;
    }

    int AppleTouchTracker::endTouch(void* touch)
    {
        for (int i = 0; i < AppleTouchTracker::MaxTouches; i++)
        {
            if (touches_[i] == touch)
            {
                touches_[i] = NULL; //clear it
                return i;
            }
        }

        Log::E("Can't end touch id\n");
        return -1;
    }

    int AppleTouchTracker::getTouchesActive()
    {
        int r = 0;

        for (int i = 0; i < AppleTouchTracker::MaxTouches; i++)
        {
            if (touches_[i] != NULL)
            {
                r++;
            }
        }

        return r;
    }
}
