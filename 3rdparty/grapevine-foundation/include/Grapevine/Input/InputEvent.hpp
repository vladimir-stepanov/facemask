/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef COMMON_INPUT_INPUTEVENT
#define COMMON_INPUT_INPUTEVENT

#include <stdint.h>

#ifdef __NGOS__
#ifndef MAX_POINTERS
#define MAX_POINTERS 10
#endif
#else /* __NGOS__  */
#define MAX_POINTERS 5
#endif

#define MAX_TOUCH_POINTS MAX_POINTERS /* FIXME: this define is used by widget gallery */

#define TOUCH_EVENT_ACTION_POINTER_INDEX_SHIFT 8

namespace Input {
    enum {
        /* Bit mask of the parts of the action code that are the action itself.
         */
        TOUCH_EVENT_ACTION_MASK = 0xff,

        /* Bits in the action code that represent a pointer index, used with
         * TOUCH_EVENT_ACTION_POINTER_DOWN and TOUCH_EVENT_ACTION_POINTER_UP.  Shifting
         * down by TOUCH_EVENT_ACTION_POINTER_INDEX_SHIFT provides the actual pointer
         * index where the data for the pointer going up or down can be found.
         */
        TOUCH_EVENT_ACTION_POINTER_INDEX_MASK  = 0xff00,

        /* A pressed gesture has started, the motion contains the initial starting location.
         */
        TOUCH_EVENT_ACTION_DOWN = 0,

        /* A pressed gesture has finished, the motion contains the final release location
         * as well as any intermediate points since the last down or move event.
         */
        TOUCH_EVENT_ACTION_UP = 1,

        /* A change has happened during a press gesture (between TOUCH_EVENT_ACTION_DOWN and
         * TOUCH_EVENT_ACTION_UP).  The motion contains the most recent point, as well as
         * any intermediate points since the last down or move event.
         */
        TOUCH_EVENT_ACTION_MOVE = 2,

        /* The current gesture has been aborted.
         * You will not receive any more points in it.  You should treat this as
         * an up event, but not perform any action that you normally would.
         */
        TOUCH_EVENT_ACTION_CANCEL = 3,

        /* A movement has happened outside of the normal bounds of the UI element.
         * This does not provide a full gesture, but only the initial location of the movement/touch.
         */
        TOUCH_EVENT_ACTION_OUTSIDE = 4,

        /* A non-primary pointer has gone down.
         * The bits in TOUCH_EVENT_ACTION_POINTER_INDEX_MASK indicate which pointer changed.
         */
        TOUCH_EVENT_ACTION_POINTER_DOWN = 5,

        /* A non-primary pointer has gone up.
         * The bits in TOUCH_EVENT_ACTION_POINTER_INDEX_MASK indicate which pointer changed.
         */
        TOUCH_EVENT_ACTION_POINTER_UP = 6,

        /* A change happened but the pointer is not down (unlike TOUCH_EVENT_ACTION_MOVE).
         * The motion contains the most recent point, as well as any intermediate points since
         * the last hover move event.
         */
        TOUCH_EVENT_ACTION_HOVER_MOVE = 7,

        /* The motion event contains relative vertical and/or horizontal scroll offsets.
         * Use getAxisValue to retrieve the information from TOUCH_EVENT_AXIS_VSCROLL
         * and TOUCH_EVENT_AXIS_HSCROLL.
         * The pointer may or may not be down when this event is dispatched.
         * This action is always delivered to the winder under the pointer, which
         * may not be the window currently touched.
         */
        TOUCH_EVENT_ACTION_SCROLL = 8,

        /* The pointer is not down but has entered the boundaries of a window or view.
         */
        TOUCH_EVENT_ACTION_HOVER_ENTER = 9,

        /* The pointer is not down but has exited the boundaries of a window or view.
         */
        TOUCH_EVENT_ACTION_HOVER_EXIT = 10,
    };

    struct InputKey
    {
        enum Enum
        {
            // Add some more here when needed
            Invalid = 0x0,
            Power_on = 0x1,
            Power_off = 0x2,
            Home = 0x3,
            Back = 0x4,
            Volume_up = 0x5,   // add for Volum key up
            Volume_down = 0x6,  //add for Volum key down
            Escape,
            ArrowUp,
            ArrowDown,
            ArrowRight,
            ArrowLeft,
            BackSpace,
            Space,
            Return,
            Shift_L,
            Number_0,
            Number_1,
            Number_2,
            Number_3,
            Number_4,
            Number_5,
            Number_6,
            Number_7,
            Number_8,
            Number_9,
            Letter_q,
            Letter_w,
            Letter_e,
            Letter_r,
            Letter_t,
            Letter_y,
            Letter_u,
            Letter_i,
            Letter_o,
            Letter_p,
            Letter_a,
            Letter_s,
            Letter_d,
            Letter_f,
            Letter_g,
            Letter_h,
            Letter_j,
            Letter_k,
            Letter_l,
            Letter_z,
            Letter_x,
            Letter_c,
            Letter_v,
            Letter_b,
            Letter_n,
            Letter_m,
            Info
        };
    };

    struct InputEventType
    {
        enum Enum
        {
            Touch,
            MouseMove,
            MouseButton,
            Keyboard,
            System,
            Unknown
        };
    };


    struct TouchFlags
    {
        enum Enum
        {
#ifdef __NGOS__
            Pressed = TOUCH_EVENT_ACTION_DOWN,
            Moved = TOUCH_EVENT_ACTION_MOVE,
            Stationary = TOUCH_EVENT_ACTION_MOVE,
            Released = TOUCH_EVENT_ACTION_UP,
            Cancel = TOUCH_EVENT_ACTION_CANCEL,
            Outside = TOUCH_EVENT_ACTION_OUTSIDE,
            NonPrimaryDown= TOUCH_EVENT_ACTION_POINTER_DOWN,
            NonPrimaryUp= TOUCH_EVENT_ACTION_POINTER_UP,
#else
            Pressed = 0x0,
            Released = 0x01,
            Moved = 0x02,
            Cancel = 0x3,
            Outside= 0x4,
            NonPrimaryDown = 0x5,
            NonPrimaryUp = 0x6,
            Stationary = 0x7,
#endif
        };
    };

    struct MouseButton
    {
        enum Enum
        {
            Left = 1,
            Middle = 2,
            Right = 3,
            WheelUp = 4,
            WheelDown = 5
        };
    };

    struct TouchPoint
    {
        unsigned int id;
        float x;
        float y;
        float lastX;
        float lastY;
        float pressure;
    };

    struct TouchEvent
    {
        InputEventType::Enum type; // Reserve space
        unsigned int flags; // 0x01 pressed, 0x02 moved, 0x04 stationary, 0x08 released
        unsigned int count;
        unsigned long long t; // timestamp (original, ms)
        unsigned long long tmStampUs; // the raw usec timestamp is pushed to be only in use from now on
        long long lastT;
        void *handle; // OS specific touch handle
        TouchPoint touchPoints[MAX_POINTERS];
    };

    struct KeyboardEvent
    {
        InputEventType::Enum type; // Reserve space
        bool keydown; // Caps & numlock: 1 when 1st pressed, 0 when released and pressed again
        bool pressed;
        InputKey::Enum symbol;
    };

    struct MouseMoveEvent
    {
        InputEventType::Enum type; // Reserve space
        uint8_t pressedBitmask; // 0 left, 1 middle, 2 right NOTE: maybe!
        unsigned int x;
        unsigned int y;
        int motionX;
        int motionY;
    };

    struct SystemEvent
    {
        InputEventType::Enum type; // Reserve space

        // TODO
    };

    struct MouseButtonEvent
    {
        InputEventType::Enum type; // Reserve space
        unsigned int deviceIndex;
        MouseButton::Enum button;
        bool pressed;
        unsigned int x;
        unsigned int y;
    };

    union InputEvent
    {
        InputEventType::Enum type; // Reserve space
        TouchEvent touch;
        MouseMoveEvent mouseMove;
        MouseButtonEvent mouseButton;
        KeyboardEvent keyboard;
        SystemEvent system;
    };
} /* namespace Input */

#endif /* COMMON_INPUT_INPUTEVENT */
