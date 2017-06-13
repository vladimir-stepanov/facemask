/**
 * Copyright (C) 2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Core/Log.hpp>
#include "InputSourcePrivateNGOS.hpp"
#include "InputSourcePrivateNGOS-Utils.h"

#ifndef SCALE_FACTOR
#define SCALE_FACTOR 2
#endif

#ifndef KEYCODE_POWER
#define KEYCODE_POWER 0x74
#endif

#ifndef KEYCODE_VOLUME_UP
#define KEYCODE_VOLUME_UP 0x73
#endif

#ifndef KEYCODE_VOLUME_DOWN
#define KEYCODE_VOLUME_DOWN 0x72
#endif

namespace Grapevine {

void InputSourcePrivateNGOS::PowerThread::run()
{
    struct input_event event;
    struct InputData power;
    int fd;

    open_powerkey_input(fd);
    if (fd < 0) {
        return;
    }

    while (running_) {
        if (wait_event(fd, &event) < 0) {
            continue;
        }
        if (event.type == EV_KEY) {
            if (event.value == 0) {
                power.state = POINTER_UP;
            } else {
                power.state = POINTER_DOWN;
            }
            power.time.tv_sec = event.time.tv_sec;
            power.time.tv_usec = event.time.tv_usec;
            power.type = KEY_DATA;
            if (event.code == KEYCODE_POWER) {
                power.code = InputKey::Power_off;
            } else {
                power.code = InputKey::Invalid;
            }

            emit(&power);
        }
    }
}

void InputSourcePrivateNGOS::VolumeThread::run()
{
    struct input_event event;
    struct InputData volume;
    int fd;

    open_volumekey_input(fd);
    if (fd < 0) {
        return;
    }

    while (running_) {
        if (wait_event(fd, &event) < 0) {
            continue;
        }
        if (event.type == EV_KEY) {
            if (event.value == 0) {
                volume.state = POINTER_UP;
            } else {
                volume.state = POINTER_DOWN;
            }
            volume.time.tv_sec = event.time.tv_sec;
            volume.time.tv_usec = event.time.tv_usec;
            volume.type = KEY_DATA;

            if (event.code == KEYCODE_VOLUME_DOWN) {
                volume.code = InputKey::Volume_down;
            } else if (event.code == KEYCODE_VOLUME_UP) {
                volume.code = InputKey::Volume_up;
            } else {
                volume.code = InputKey::Invalid;
            }

            emit(&volume);
        }
    }
}

static void prepDown(struct InputData *touch, struct input_event *event, uint8_t slot)
{
    touch->time.tv_sec = event->time.tv_sec;
    touch->time.tv_usec = event->time.tv_usec;
    touch->type = MOTION_DATA;
    touch->state = POINTER_DOWN;
    touch->points[slot].flags |= POINTER_FLG_DOWN;
    touch->index = slot;
    touch->points[slot].id = slot;
    touch->slots[slot] = slot;
}

static void prepUp(struct InputData *touch, struct input_event *event, uint8_t slot)
{
    touch->time.tv_sec = event->time.tv_sec;
    touch->time.tv_usec = event->time.tv_usec;
    touch->type = MOTION_DATA;
    touch->state = POINTER_UP;
    touch->index = slot;
    touch->points[slot].id = slot;
    touch->slots[slot] = slot;
}

static void postUp(struct InputData *touch, uint8_t slot)
{
    touch->points[slot].flags = 0;
    /* hervanta expects no gaps and incremental order of reported events */
    touch->slots[slot] = touch->slots[slot + 1]; /* fill gap */
}

static void prepMove(struct InputData *touch, struct input_event *event, uint8_t slot)
{
    touch->time.tv_sec = event->time.tv_sec;
    touch->time.tv_usec = event->time.tv_usec;
    touch->state = POINTER_MOVE;
    touch->type = MOTION_DATA;
    touch->index = slot;
    touch->slots[slot] = slot;
}

void InputSourcePrivateNGOS::TouchThread::run()
{
    uint8_t p;
    uint16_t x, y;
    uint8_t slots[MAX_POINTERS]; /* currently active slots */
    struct InputData touch;
    struct input_event event;
    int fd;

    open_touch_input(fd);
    if (fd < 0) {
        return;
    }

    memset(&touch, 0, sizeof(touch));
    memset(touch.slots, -1, sizeof(touch.slots));
    memset(slots, 0, sizeof(slots));
    touch.code = InputKey::Invalid; /* always for touch */
    touch.pressure_max = 1;
    x = y = p = 0;

    while (running_) {
        int rc = wait_event(fd, &event);
        if (rc < 0) {
            continue;
        }

        trace_event(&event, 0);

        if (event.type == EV_SYN) {
            if (!event.code) {
                touch.flags |= POINTER_FLG_DONE;
            }
        } else if (event.type == EV_ABS) {
            if (event.code == ABS_MT_PRESSURE) {
                if (event.value > touch.pressure_max) { /* "self-calibrate" */
                    touch.pressure_max = event.value * SCALE_FACTOR;
                }
                p = event.value;
            } else if (event.code == ABS_MT_POSITION_X) {
                x = event.value;
            } else if (event.code == ABS_MT_POSITION_Y) {
                y = event.value;
            } else if (event.code == ABS_MT_TRACKING_ID && event.value >= 0) {
                touch.points[event.value].x = x;
                touch.points[event.value].y = y;
                touch.points[event.value].p = p;
                touch.points[event.value].id = event.value;
                slots[event.value] = 1;

                if (touch.points[event.value].flags & POINTER_FLG_DOWN) {
                    prepMove(&touch, &event, event.value);
                    emit(&touch);
                } else {
                    prepDown(&touch, &event, event.value);
                    emit(&touch);
                    touch.count++;
                }
            }
        }

        if (touch.flags & POINTER_FLG_DONE) {
            /* Grace hardware does not indicate explicitly that slot has been
             * released hence this loop :( ... */
            uint8_t i;
            for (i = 0; i < MAX_POINTERS; ++i) { /* clean inactive slots if any */
                if (!touch.count) {
                    memset(touch.slots, -1, sizeof(touch.slots));
                    break;
                }

                if (!slots[i] && (touch.points[i].flags & POINTER_FLG_DOWN)) { /* now released */
                    prepUp(&touch, &event, i);
                    emit(&touch);
                    postUp(&touch, i);
                    if (touch.count) {
                        touch.count--;
                    }
                }
            }
            memset(slots, 0, sizeof(slots));
            touch.flags = 0;
        }
    }
}

} /* namespace Grapevine */
