/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_INPUT_APPLETOUCHTRACKER
#define GRAPEVINE_INPUT_APPLETOUCHTRACKER

namespace Grapevine
{
    class AppleTouchTracker
    {
    public:
        static unsigned int const MaxTouches = 4; // Older devices have only 4

        AppleTouchTracker () {};

        int getId(void* touch);
        int addNewTouch(void* touch);
        int endTouch(void* touch);
        int getTouchesActive();

    private:
        void* touches_[MaxTouches];
    };
}

#endif
