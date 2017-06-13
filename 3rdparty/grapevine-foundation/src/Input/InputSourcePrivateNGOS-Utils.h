/**
 * Copyright (C) 2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Core/Log.hpp>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/input.h>

#ifndef ABS_MT_POSITION_X
#define ABS_MT_POSITION_X 0x35
#endif

#ifndef ABS_MT_POSITION_Y
#define ABS_MT_POSITION_Y 0x36
#endif

#ifndef ABS_MT_TRACKING_ID
#define ABS_MT_TRACKING_ID 0x39
#endif

#ifndef ABS_MT_PRESSURE
#define ABS_MT_PRESSURE 0x3a
#endif

#ifndef ABS_MT_TOUCH_MAJOR
#define ABS_MT_TOUCH_MAJOR 0x30
#endif

#ifndef ABS_MT_SLOT
#define ABS_MT_SLOT 0x2f
#endif

#define EVTAG "evtag"

#ifndef POWER_DEV
#define POWER_DEV "p"
#endif

#ifndef VOLUME_DEV
#define VOLUME_DEV "v"
#endif

#ifndef TOUCH_DEV
#define TOUCH_DEV "t"
#endif

#ifndef INPUT_MAXDEV
#define INPUT_MAXDEV 255
#endif

#ifndef INPUT_BASEDIR
#define INPUT_BASEDIR "/dev/input"
#endif

#ifndef INPUT_DEVPREFIX
#define INPUT_DEVPREFIX "event"
#endif

#ifdef POWER_NAME
#define open_powerkey_input(fd) fd = open_device_by_name(POWER_NAME)
#else
#define open_powerkey_input(fd) fd = open_device(POWER_DEV)
#endif

#ifdef VOLUME_NAME
#define open_volumekey_input(fd) fd = open_device_by_name(VOLUME_NAME)
#else
#define open_volumekey_input(fd) fd = open_device(VOLUME_DEV)
#endif

#ifdef TOUCH_NAME
#define open_touch_input(fd) fd = open_device_by_name(TOUCH_NAME)
#else
#define open_touch_input(fd) fd = open_device(TOUCH_DEV)
#endif

static int open_device_by_name(const char *devname)
{
    int fd;
    unsigned int i;
    char name[32];
    char path[1024];

    fd = -1;
    for (i = 0; i < INPUT_MAXDEV; i++) {
        snprintf(path, sizeof(path), INPUT_BASEDIR "/" INPUT_DEVPREFIX "%u", i);

        if ((fd = open(path, O_RDONLY)) < 0) {
            LOGE(EVTAG" open(%s) failed, %s", path, strerror(errno));
            continue;
        }

        memset(name, 0, sizeof(name));
        if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
            LOGE(EVTAG" ioctl(%s, EVIOCGNAME) failed, %s", path, strerror(errno));
        } else if (strcmp(name, devname) == 0) {
            if (fcntl(fd, F_SETFL, O_NONBLOCK)) {
                LOGE(EVTAG" fcntl(%s, O_NONBLOCK) failed, %s", path, strerror(errno));
            }
            LOGI(EVTAG" fd %d, path %s, name '%s'", fd, path, name);
            break;
        }
        close(fd);
        fd = -1;
    }

    return fd;
}

static int open_device(const char *path)
{
    char name[32] = { 0 };
    int fd = open(path, O_RDONLY);

    if (fd < 0) {
        LOGE(EVTAG" open(%s) failed, %s", path, strerror(errno));
        return -1;
    }

    /* just for information */
    if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
        LOGE(EVTAG" ioctl(%s, EVIOCGNAME) failed, %s", path, strerror(errno));
        name[0] = '\0';
    }

    LOGI(EVTAG" path %s, name '%s'", path, name);

    if (fcntl(fd, F_SETFL, O_NONBLOCK)) {
        LOGE(EVTAG" fcntl(%s, O_NONBLOCK) failed, %s", path, strerror(errno));
        goto err;
    }

    return fd;
err:
    close(fd);
    return -1;
}

/* @ret -1 continue, 0 received, 1 completed */
static int wait_event(int fd, struct input_event *event)
{
    struct pollfd pfd = { .fd = fd, .events = POLLIN, .revents = 0, };
    int rc = poll(&pfd, 1, -1);

    if (rc == 0) { /* timeout */
        return -1;
    } else if (rc < 0) {
        if (errno == EINTR) { /* just ignore it */
            return -1;
        }
        sleep(1); /* relax and try again */
        return -1;
    }

    if (!(pfd.revents & POLLIN)) {
        LOGI(EVTAG" fd %d not ready", pfd.fd);
        return -1;
    }

    errno = 0;
    if (read(pfd.fd, event, sizeof(*event)) != sizeof(*event)) {
        if (errno == EAGAIN) {
            return 1; /* possible event completion */
        }
        LOGE(EVTAG" read(%d) failed, %s", pfd.fd, strerror(errno));
    }
    return 0;
}

#ifndef TRACE_EVENTS
#define event_print(a, b, c) do {} while(0)
#define trace_event(a, b) do {} while(0)
#else
static void event_print(struct input_event *e, const char *name, int slot)
{
    const char *code;

    if (e->code == ABS_MT_TOUCH_MAJOR) {
        code = "ABS_MT_TOUCH_MAJOR";
    } else if (e->code == ABS_MT_PRESSURE) {
        code = "ABS_MT_PRESSURE";
    } else if (e->code == ABS_MT_POSITION_X) {
        code = "ABS_MT_POSITION_X";
    } else if (e->code == ABS_MT_POSITION_Y) {
        code = "ABS_MT_POSITION_Y";
    } else if (e->code == ABS_MT_TRACKING_ID) {
        code = "ABS_MT_TRACKING_ID";
    } else if (e->code == ABS_MT_SLOT) {
        code = "ABS_MT_SLOT";
    } else {
        code = "<skipped>";
    }

    LOGI(EVTAG" %ld.%ld %s type 0x%x code 0x%x (%s) value %d slot %d",
         e->time.tv_sec, e->time.tv_usec, name, e->type, e->code, code,
         e->value, slot);
}

static void trace_event(struct input_event *event, int slot)
{
    /* we are only interested in these events */
    if (event->type == EV_KEY) {
        event_print(event, "EV_KEY", slot);
    } else if (event->type == EV_SYN) {
        event_print(event, "EV_SYN", slot);
    } else if (event->type == EV_REL) {
        event_print(event, "EV_REL", slot);
    } else if (event->type == EV_ABS) {
        event_print(event, "EV_ABS", slot);
    } else if (event->type == EV_PWR) {
        event_print(event, "EV_PWR", slot);
    } else if (event->type == EV_SYN) {
        /* ignore */
    } else {
        LOGI(EVTAG" %ld.%ld type 0x%x code 0x%x value %d slot %d",
	     event->time.tv_sec, event->time.tv_usec,
             event->type, event->code, event->value, slot);
    }
}
#endif
