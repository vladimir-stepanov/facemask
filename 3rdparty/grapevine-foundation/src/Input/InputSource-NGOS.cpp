/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Input/InputSource.hpp"
#include "InputSourcePrivateNGOS.hpp"

namespace Grapevine
{
    InputSource::InputSource(Canvas& canvas):
        private_(new InputSourcePrivateNGOS())
    {
    }

    void InputSource::addEvent(InputEvent* event)
    {
        private_->addEvent(event);
    }

    bool InputSource::pollEvent(InputEvent& event)
    {
        return private_->pollEvent(event);
    }

    void InputSource::connectOnEvent(sigc::slot<void, InputEvent *> onEvent)
    {
        private_->connectOnEvent(onEvent);
    }
}
