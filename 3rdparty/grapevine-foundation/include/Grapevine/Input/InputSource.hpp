/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_INPUT_INPUTSOURCE
#define GRAPEVINE_INPUT_INPUTSOURCE

#include <sigc++/sigc++.h>

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputEvent.hpp>

using namespace Input;

namespace Grapevine
{
    class InputSourcePrivate;

    class InputSource
    {
    public:
        CLASS_UNCOPYABLE(InputSource);

        InputSource(Canvas& canvas);
        void addEvent(InputEvent* event);
        bool pollEvent(InputEvent& event);
        void connectOnEvent(sigc::slot<void, InputEvent *> onEvent);

    private:
        InputSourcePrivate* private_;
    };
}

#endif
