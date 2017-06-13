/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "InputSourcePrivate.hpp"
#include <Grapevine/Core/Lock.hpp>

using namespace Input;

namespace Grapevine
{
    InputSourcePrivate::InputSourcePrivate()
    {
    }

    void InputSourcePrivate::addEvent(InputEvent* event)
    {
        LOCK
        queue_.push(event);
        onEvent_.emit(event);
    }

    bool InputSourcePrivate::pollEvent(InputEvent& event)
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

    void InputSourcePrivate::connectOnEvent(sigc::slot<void, InputEvent *> onEvent)
    {
        onEvent_.connect(onEvent);
    }

}
