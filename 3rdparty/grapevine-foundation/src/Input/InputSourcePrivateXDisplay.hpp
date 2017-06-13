/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_INPUT_INPUTSOURCEPRIVATEXDISPLAY
#define GRAPEVINE_INPUT_INPUTSOURCEPRIVATEXDISPLAY

#include <Grapevine/Core/Mutex.hpp>
#include "InputSourcePrivate.hpp"
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace Grapevine
{
    class InputSourcePrivateXDisplay: public InputSourcePrivate
    {
    public:
        InputSourcePrivateXDisplay(Display* xDisplay);
        bool pollEvent(InputEvent& event);
        void addEvent(InputEvent* event);

    private:
        bool pollInputEvent(InputEvent& event);

        Display* xDisplay_;
        Mutex mutex_;
        std::queue<InputEvent*> queue_;
    };
}

#endif
