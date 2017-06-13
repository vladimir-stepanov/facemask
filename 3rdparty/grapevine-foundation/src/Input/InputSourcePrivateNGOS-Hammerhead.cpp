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

    fd = open_device("/dev/input/event0");
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

    fd = open_device("/dev/input/event2");
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
    int16_t tmp;

    touch->time.tv_sec = event->time.tv_sec;
    touch->time.tv_usec = event->time.tv_usec;
    touch->type = MOTION_DATA;
    touch->state = POINTER_DOWN;
    touch->points[slot].flags |= POINTER_FLG_DOWN;
    touch->count++;
    touch->points[slot].flags &= ~POINTER_FLG_MOVE;
    touch->index = slot;
    touch->points[slot].id = slot;

    if (touch->slots[slot] != slot) { /* swap with other slot */
        tmp = touch->slots[slot];
        touch->slots[slot] = slot;
        if (tmp != -1) {
            touch->slots[tmp] = tmp;
        }
    }
    /* this is insane and hervanta must be changed: currently we adopted Android
     * way of handling multitouch events.. but for now fill gaps... */
    for (tmp = slot; tmp > 0; tmp--) {
        if (touch->slots[tmp] < 0 && touch->slots[tmp - 1] > slot) {
            touch->slots[tmp] = slot;
        } else if (touch->slots[tmp] < 0 && touch->slots[tmp - 1] < slot) {
            touch->slots[tmp] = touch->slots[tmp - 1];
        }
    }
}

static void prepUp(struct InputData *touch, struct input_event *event, uint8_t slot)
{
    touch->time.tv_sec = event->time.tv_sec;
    touch->time.tv_usec = event->time.tv_usec;
    touch->type = MOTION_DATA;
    touch->state = POINTER_UP;
    touch->index = slot;
    touch->points[slot].id = slot;
}

static void postUp(struct InputData *touch, uint8_t slot)
{
    touch->points[slot].flags &= ~POINTER_FLG_DOWN;
    touch->points[slot].flags &= ~POINTER_FLG_MOVE;
    touch->slots[slot] = touch->slots[slot + 1];

    if (touch->count) {
        touch->count--;
    }

    if (!touch->count) { /* make sure no leftovers */
        memset(touch->slots, -1, sizeof(touch->slots));
    }
}

void InputSourcePrivateNGOS::TouchThread::run()
{
    uint8_t slot;
    struct InputData touch;
    struct input_event event;
    int fd;

    fd = open_device("/dev/input/event1");
    if (fd < 0) {
        return;
    }

    memset(&touch, 0, sizeof(touch));
    memset(touch.slots, -1, sizeof(touch.slots));
    touch.code = InputKey::Invalid; /* always for touch */
    touch.pressure_max = 1;
    slot = 0;

    while (running_) {
        int rc = wait_event(fd, &event);
        if (rc < 0) {
            continue;
        }

        trace_event(&event, slot);

        if (event.type == EV_SYN) {
            touch.flags |= POINTER_FLG_DONE;
        } else if (event.type == EV_ABS) {
            if (event.code == ABS_MT_PRESSURE) {
                if (event.value > touch.pressure_max) { /* "self-calibrate" */
                    touch.pressure_max = event.value * SCALE_FACTOR;
                }
                touch.points[slot].p = event.value;
                if (!(touch.points[slot].flags & POINTER_FLG_DOWN)) {
                    prepDown(&touch, &event, slot);
                    emit(&touch);
                }
            } else if (event.code == ABS_MT_POSITION_X) {
                touch.points[slot].x = event.value;
                touch.points[slot].flags |= POINTER_FLG_MOVE;
                touch.flags |= POINTER_FLG_MOVE;
            } else if (event.code == ABS_MT_POSITION_Y) {
                touch.points[slot].y = event.value;
                touch.points[slot].flags |= POINTER_FLG_MOVE;
                touch.flags |= POINTER_FLG_MOVE;
            } else if (event.code == ABS_MT_SLOT) {
                if (event.value < MAX_POINTERS) {
                    slot = event.value;
                    touch.flags &= ~POINTER_FLG_ERR;
                } else {
                    LOGE(EVTAG" support is limited to %d events max", MAX_POINTERS);
                    continue;
                    slot = 0;
                    touch.flags = POINTER_FLG_ERR;
                }
            } else if (event.code == ABS_MT_TRACKING_ID) {
                if (event.value < 0) {
                    prepUp(&touch, &event, slot);
                    emit(&touch);
                    postUp(&touch, slot);
                }
            }
        }

        if (touch.flags & POINTER_FLG_DONE) {
            if (touch.flags & POINTER_FLG_MOVE) {
                /* these checks are needed because sometimes slot and pressure
                 * values do not arrive within same frame */
                if ((touch.points[0].flags & POINTER_FLG_MOVE) &&
                    !(touch.points[0].flags & POINTER_FLG_DOWN)) {
                    prepDown(&touch, &event, 0);
                    emit(&touch);
                }

                if (touch.points[slot].flags & POINTER_FLG_MOVE) {
                    if (!(touch.points[slot].flags & POINTER_FLG_DOWN)) {
                        prepDown(&touch, &event, slot);
                        emit(&touch);
                    } else {
                        touch.time.tv_sec = event.time.tv_sec;
                        touch.time.tv_usec = event.time.tv_usec;
                        touch.state = POINTER_MOVE;
                        touch.type = MOTION_DATA;
                        touch.index = slot;
                        emit(&touch);
                        touch.points[slot].flags &= ~POINTER_FLG_MOVE;
                    }
                }
            }
            touch.flags = 0;
        }
    }
}

} /* namespace Grapevine */
