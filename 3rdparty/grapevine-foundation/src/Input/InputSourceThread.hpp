/**
 * Copyright (C) 2016 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_INPUT_INPUTSOURCETHREAD
#define GRAPEVINE_INPUT_INPUTSOURCETHREAD

#include <Grapevine/Core/Thread.hpp>
#include <Grapevine/Core/Log.hpp>

#include "Grapevine/Input/InputEvents.hpp"

/* FIXME: move this stuff into header above */
#ifdef __NGOS__
#ifndef MAX_POINTERS
#define MAX_POINTERS 10
#endif
#else /* __NGOS__  */
#define MAX_POINTERS 5
#endif

#define POINTER_FLG_DOWN (1 << 0) /* has been active since across EV_SYNs frames */
#define POINTER_FLG_MOVE (1 << 1)
#define POINTER_FLG_DONE (1 << 2)
#define POINTER_FLG_ERR (1 << 3)

using namespace Input;

namespace Grapevine {

enum EventType {
    MOTION_DATA,
    KEY_DATA
};

enum PointerState {
    POINTER_DOWN,
    POINTER_MOVE,
    POINTER_UP,
};

struct PointerData { /* p,x,y originals are all int32_t */
    uint8_t flags;
    uint8_t p;
    int16_t x;
    int16_t y;
    uint16_t id; /* change type if more ids are needed */
};

struct InputData {
    struct timeval time;
    struct PointerData points[MAX_POINTERS];
    int16_t slots[MAX_POINTERS + 1];
    uint16_t index; /* active point index */
    uint16_t count; /* number of active points */
    uint8_t pressure_max;
    uint8_t flags; /* POINTER_FLG_* */
    enum EventType type;
    enum PointerState state;
    InputKey::Enum code;
};

typedef void (inputSourceThreadCb)(void *, InputData *);

class InputSourceThread : public Thread {
    public:
        InputSourceThread(void *parent, inputSourceThreadCb *cb)
        {
            parent_ = parent;
            cb_ = cb;
            running_ = true;
        };
        ~InputSourceThread() { running_ = false; };
        void stop() { running_ = false; };
        void emit(InputData *data) { cb_(parent_, data); };
        bool running_;
    protected:
        virtual void run() = 0;
    private:
        void *parent_;
        inputSourceThreadCb *cb_;
};

} /* namespace Grapevine */

#endif /* GRAPEVINE_INPUT_INPUTSOURCETHREAD */
