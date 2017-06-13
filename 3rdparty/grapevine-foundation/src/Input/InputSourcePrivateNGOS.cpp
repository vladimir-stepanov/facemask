/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "InputSourcePrivateNGOS.hpp"
#include "Grapevine/Input/InputEvents.hpp"
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Lock.hpp>
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Timer.hpp> // tracing
#include <queue>
#include <unistd.h>

//#include <utils/CallStack.h>
#include <stdio.h>

#ifndef TOUCH_EVENT_ACTION_POINTER_INDEX_SHIFT
#define TOUCH_EVENT_ACTION_POINTER_INDEX_SHIFT 8
#endif

#define EVTAG "evtag"
//#define TRACELOG_PERF

using namespace Input;

namespace Grapevine
{
    namespace
    {
        InputSourcePrivateNGOS *InputSourcePrivateNGOS_;
        void InputHandle(void *parent, struct InputData *data)
        {
            InputSourcePrivateNGOS *input = (InputSourcePrivateNGOS *) parent;
            float scale;
            InputEvent event;
            uint16_t i;
            uint64_t ts_usec;

            switch (data->type) {
            case MOTION_DATA:

                event.touch.t = data->time.tv_sec * 1000ULL + data->time.tv_usec / 1000U;

                // Solely original raw [us] timestamping is promoted to be only in use from now on ;)
                event.touch.tmStampUs = data->time.tv_sec * 1000000ULL + data->time.tv_usec;

                event.touch.lastT = 0.0; // FIXME: What is the purpose of assigning float to 2-long int?  Where it's used and for what?
                event.touch.handle = NULL;
                event.type = InputEventType::Touch;

                scale = 1.f / data->pressure_max;

                event.touch.count = 0;

                for (i = 0; i < MAX_POINTERS; i++) {
                    if (data->slots[i] < 0) {
                        continue;
                    } else if (i == data->index) {
                        event.touch.flags = event.touch.count << TOUCH_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    }
                    int slot = data->slots[i];
                    event.touch.touchPoints[event.touch.count].id = data->points[slot].id;
                    event.touch.touchPoints[event.touch.count].x = data->points[slot].x;
                    event.touch.touchPoints[event.touch.count].y = data->points[slot].y;
                    event.touch.touchPoints[event.touch.count].pressure = scale * data->points[slot].p;
                    event.touch.touchPoints[event.touch.count].lastX = 0.0;
                    event.touch.touchPoints[event.touch.count].lastY = 0.0;
                    event.touch.count++;
                }

                if (data->state == POINTER_DOWN) {
                    event.touch.flags |= TouchFlags::Pressed;
                } else if (data->state == POINTER_MOVE) {
                    event.touch.flags = TouchFlags::Moved;
                } else if (data->state == POINTER_UP) {
                    event.touch.flags |= TouchFlags::Released;
                } else {
                    event.touch.flags |= TouchFlags::Cancel;
                    LOGE(EVTAG" %ld.%ld unknown touch state", data->time.tv_sec, data->time.tv_usec);
                }

#ifdef TRACELOG_PERF
                ts_usec = Timer::autoTimerNanosecs() / 1000U;
                LOGI("[ -MSG_DELAYS- ]<GrFND> (EvTStmp: --ms | %llu us) {( ev.handle delay: %llu us )} %llu us in InputSourcePrivateNGOS. |Touch-Flags:%u|",
                    event.touch.tmStampUs, ts_usec - event.touch.tmStampUs, ts_usec, event.touch.flags);
#endif
                input->addEvent(&event);
                break;
            case KEY_DATA:
                event.type = InputEventType::Keyboard;
                event.keyboard.symbol = data->code;

                if (data->state == POINTER_DOWN) {
                    event.keyboard.pressed = true;
                } else {
                    event.keyboard.pressed = false;
                }

                input->addEvent(&event);
                break;
            }
        }
    }

    InputSourcePrivateNGOS::InputSourcePrivateNGOS() :
        InputSourcePrivate(),
        powerThread_(this, InputHandle),
        volumeThread_(this, InputHandle),
        touchThread_(this, InputHandle)
    {
        InputSourcePrivateNGOS_ = this; /* should be set before starting threads */
        powerThread_.start();
        volumeThread_.start();
        touchThread_.start();
    }

    InputSourcePrivateNGOS::~InputSourcePrivateNGOS()
    {
        powerThread_.stop();
        volumeThread_.stop();
        touchThread_.stop();
        powerThread_.join();
        volumeThread_.join();
        touchThread_.join();
    }

    void InputSourcePrivateNGOS::addEvent(InputEvent* event)
    {
        {
            LOCK
            queue_.push(*event);
        }
        onEvent_.emit(event);
    }

    bool InputSourcePrivateNGOS::pollEvent(InputEvent& event)
    {

        LOCK
        if (queue_.empty() == true)
        {
            return false;
        }

        event = queue_.front();
        queue_.pop();

        return true;
    }

} // namespace Grapevine
