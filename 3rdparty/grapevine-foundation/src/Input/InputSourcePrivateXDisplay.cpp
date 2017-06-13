/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "InputSourcePrivateXDisplay.hpp"
#include <Grapevine/Core/Lock.hpp>

namespace Grapevine
{
    InputSourcePrivateXDisplay::InputSourcePrivateXDisplay(Display* xDisplay):
        InputSourcePrivate(),
        xDisplay_(xDisplay)
    {
    }

    bool InputSourcePrivateXDisplay::pollEvent(InputEvent& event)
    {
        if(pollInputEvent(event))
        {
            return true;
        }

        XEvent xEvent;
        bool r = false;

        if (xDisplay_ == NULL)
        {
            return false;
        }
        if (!XPending(xDisplay_))
        {
            return false;
        }

        XNextEvent(xDisplay_, &xEvent);

        if (xEvent.type == KeyPress || xEvent.type == KeyRelease)
        {
            r = true;
            event.type = InputEventType::Keyboard;
            event.keyboard.pressed = xEvent.type == KeyPress ? 1 : 0;
            if (XLookupKeysym(&xEvent.xkey, 0) == XK_Escape)
            {
                event.keyboard.symbol = InputKey::Escape;
                Log::D("KeyPress: XK_Escape");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_Up)
            {
                event.keyboard.symbol = InputKey::ArrowUp;
                Log::D("KeyPress: XK_Up");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_Down)
            {
                event.keyboard.symbol = InputKey::ArrowDown;
                Log::D("KeyPress: XK_Down");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_Left)
            {
                event.keyboard.symbol = InputKey::ArrowLeft;
                Log::D("KeyPress: XK_Left");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_Right)
            {
                event.keyboard.symbol = InputKey::ArrowRight;
                Log::D("KeyPress: XK_Right");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_BackSpace)
            {
                event.keyboard.symbol = InputKey::BackSpace;
                Log::D("KeyPress: XK_BackSpace");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_space)
            {
                event.keyboard.symbol = InputKey::Space;
                Log::D("KeyPress: XK_space");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_Return)
            {
                event.keyboard.symbol = InputKey::Return;
                Log::D("KeyPress: XK_Return");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_Shift_L)
            {
                event.keyboard.symbol = InputKey::Shift_L;
                Log::D("KeyPress: XK_Shift_L");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_0)
            {
                event.keyboard.symbol = InputKey::Number_0;
                Log::D("KeyPress: XK_0");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_1)
            {
                event.keyboard.symbol = InputKey::Number_1;
                Log::D("KeyPress: XK_1");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_2)
            {
                event.keyboard.symbol = InputKey::Number_2;
                Log::D("KeyPress: XK_2");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_3)
            {
                event.keyboard.symbol = InputKey::Number_3;
                Log::D("KeyPress: XK_3");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_4)
            {
                event.keyboard.symbol = InputKey::Number_4;
                Log::D("KeyPress: XK_4");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_5)
            {
                event.keyboard.symbol = InputKey::Number_5;
                Log::D("KeyPress: XK_5");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_6)
            {
                event.keyboard.symbol = InputKey::Number_6;
                Log::D("KeyPress: XK_6");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_7)
            {
                event.keyboard.symbol = InputKey::Number_7;
                Log::D("KeyPress: XK_7");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_8)
            {
                event.keyboard.symbol = InputKey::Number_8;
                Log::D("KeyPress: XK_8");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_9)
            {
                event.keyboard.symbol = InputKey::Number_9;
                Log::D("KeyPress: XK_9");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_q)
            {
                event.keyboard.symbol = InputKey::Letter_q;
                Log::D("KeyPress: q");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_w)
            {
                event.keyboard.symbol = InputKey::Letter_w;
                Log::D("KeyPress: w");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_e)
            {
                event.keyboard.symbol = InputKey::Letter_e;
                Log::D("KeyPress: e");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_r)
            {
                event.keyboard.symbol = InputKey::Letter_r;
                Log::D("KeyPress: r");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_t)
            {
                event.keyboard.symbol = InputKey::Letter_t;
                Log::D("KeyPress: t");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_y)
            {
                event.keyboard.symbol = InputKey::Letter_y;
                Log::D("KeyPress: y");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_u)
            {
                event.keyboard.symbol = InputKey::Letter_u;
                Log::D("KeyPress: u");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_i)
            {
                event.keyboard.symbol = InputKey::Letter_i;
                Log::D("KeyPress: i");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_o)
            {
                event.keyboard.symbol = InputKey::Letter_o;
                Log::D("KeyPress: o");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_p)
            {
                event.keyboard.symbol = InputKey::Letter_p;
                Log::D("KeyPress: p");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_a)
            {
                event.keyboard.symbol = InputKey::Letter_a;
                Log::D("KeyPress: a");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_s)
            {
                event.keyboard.symbol = InputKey::Letter_s;
                Log::D("KeyPress: s");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_d)
            {
                event.keyboard.symbol = InputKey::Letter_d;
                Log::D("KeyPress: d");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_f)
            {
                event.keyboard.symbol = InputKey::Letter_f;
                Log::D("KeyPress: f");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_g)
            {
                event.keyboard.symbol = InputKey::Letter_g;
                Log::D("KeyPress: g");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_h)
            {
                event.keyboard.symbol = InputKey::Letter_h;
                Log::D("KeyPress: h");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_j)
            {
                event.keyboard.symbol = InputKey::Letter_j;
                Log::D("KeyPress: j");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_k)
            {
                event.keyboard.symbol = InputKey::Letter_k;
                Log::D("KeyPress: k");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_l)
            {
                event.keyboard.symbol = InputKey::Letter_l;
                Log::D("KeyPress: l");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_z)
            {
                event.keyboard.symbol = InputKey::Letter_z;
                Log::D("KeyPress: z");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_x)
            {
                event.keyboard.symbol = InputKey::Letter_x;
                Log::D("KeyPress: x");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_c)
            {
                event.keyboard.symbol = InputKey::Letter_c;
                Log::D("KeyPress: c");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_v)
            {
                event.keyboard.symbol = InputKey::Letter_v;
                Log::D("KeyPress: v");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_b)
            {
                event.keyboard.symbol = InputKey::Letter_b;
                Log::D("KeyPress: b");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_n)
            {
                event.keyboard.symbol = InputKey::Letter_n;
                Log::D("KeyPress: n");
            }
            else if (XLookupKeysym(&xEvent.xkey, 0) == XK_m)
            {
                event.keyboard.symbol = InputKey::Letter_m;
                Log::D("KeyPress: m");
            }
            else
            {
                r = false;
            }
        }
        else if (xEvent.type == ClientMessage)
        {   // Delete window
            event.type = InputEventType::System;

            Log::I("Event: ClientMessage");
            r = true;
        }
        else if (xEvent.type == DestroyNotify)
        {   // Delete window
            event.type = InputEventType::System;

            Log::I("Event: DestroyNotify");
            r = true;
        }
        else if (xEvent.type == ConfigureNotify)
        {
            event.type = InputEventType::System;

            Log::I("Event: ConfigureNotify!\n");
            r = true;
        }
        else if (xEvent.type == MotionNotify || xEvent.type == ButtonPress || xEvent.type == ButtonRelease)
        {
            static bool lastXYAvailable = false;
            static bool buttonPressed = false;
            static unsigned int lastX = 0;
            static unsigned int lastY = 0;

            event.type = InputEventType::Touch;
            event.touch.count = 1;
            event.touch.touchPoints[0].pressure = 1.0f;
            event.touch.touchPoints[0].id = 0;  // NOTE: no multitouch, id always the same 0
            event.touch.touchPoints[0].x = xEvent.xmotion.x;
            event.touch.touchPoints[0].y = xEvent.xmotion.y;
            if (lastXYAvailable == true)
            {
                event.touch.touchPoints[0].lastX = lastX;
                event.touch.touchPoints[0].lastY = lastY;
            }

            unsigned int touchFlags = 0;

            if (buttonPressed && xEvent.type == MotionNotify)
            {
                touchFlags = TouchFlags::Moved;
            }

            if (xEvent.type == ButtonPress)
            {
                touchFlags = TouchFlags::Pressed;
                buttonPressed = true;
            }

            if (xEvent.type == ButtonRelease)
            {
                touchFlags = TouchFlags::Released;
                buttonPressed = false;
            }

/*
            if (lastX == xEvent.xmotion.x && lastY == xEvent.xmotion.y)
            {
                touchFlags += TouchFlags::Stationary;
            }
*/

            event.touch.flags = touchFlags;

            if(touchFlags || buttonPressed)
            {
                lastX = xEvent.xmotion.x;
                lastY = xEvent.xmotion.y;
                lastXYAvailable = true;
                r = true;
            }
        }

    end:
        return r;
    }

    void InputSourcePrivateXDisplay::addEvent(InputEvent* event)
    {
        LOCK
        queue_.push(event);
        onEvent_.emit(event);
    }

    bool InputSourcePrivateXDisplay::pollInputEvent(InputEvent& event)
    {
        LOCK

        if (queue_.empty() == true)
        {
            return false;
        }

        InputEvent* r = queue_.front();
        queue_.pop();

        event = *r;

        delete r;

        return true;
    }
}
