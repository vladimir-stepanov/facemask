/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_INPUT_INPUTSOURCEPRIVATE
#define GRAPEVINE_INPUT_INPUTSOURCEPRIVATE

#include <sigc++/sigc++.h>

#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputEvent.hpp>
#include <Grapevine/Core/Mutex.hpp>
#include <queue>

using namespace Input;

namespace Grapevine
{
    class InputSourcePrivate
    {
    public:
        InputSourcePrivate();
        virtual void addEvent(InputEvent* event);
        virtual bool pollEvent(InputEvent& event);
        virtual ~InputSourcePrivate() {};
        virtual void connectOnEvent(sigc::slot<void, InputEvent *> onEvent);

    protected:
        sigc::signal<void, InputEvent *> onEvent_;

    private:
        Mutex mutex_;
        std::queue<InputEvent*> queue_;
    };
}

#endif
