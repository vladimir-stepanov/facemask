/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Input/InputSource.hpp"
#include "InputSourcePrivate.hpp"
#include "../Canvas/CanvasPrivateSDL.hpp"

namespace Grapevine
{
    InputSource::InputSource(Canvas& canvas):
        private_(new InputSourcePrivate())
    {
    }

    void InputSource::addEvent(InputEvent* event)
    {
        private_->addEvent(event);
    }

    bool InputSource::pollEvent(InputEvent& event)
    {
        static bool fingerDown = false;
        SDL_Event sdlEvent;
        event.type = InputEventType::Unknown;

        if(private_->pollEvent(event))
        {
            return true;
        }

        if(SDL_PollEvent(&sdlEvent))
        {
            switch(sdlEvent.type)
            {
                case SDL_QUIT:
                    event.type = InputEventType::System;
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(fingerDown)
                    {
                        fingerDown = false;
                        event.touch.flags = TouchFlags::Released;
                        event.type = InputEventType::Touch;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(!fingerDown)
                    {
                        fingerDown = true;
                        event.touch.flags = TouchFlags::Pressed;
                        event.type = InputEventType::Touch;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if(fingerDown)
                    {
                        event.touch.flags = TouchFlags::Moved;
                        event.type = InputEventType::Touch;
                    }
                  break;
                default:
                    break;
            }

            if(event.type == InputEventType::Touch){
                event.touch.touchPoints[0].id = 0;
                event.touch.touchPoints[0].pressure = 1.0f;
                event.touch.touchPoints[0].x = sdlEvent.button.x;
                event.touch.touchPoints[0].y = sdlEvent.button.y;
                event.touch.touchPoints[0].lastX = sdlEvent.button.x;
                event.touch.touchPoints[0].lastY = sdlEvent.button.y;
                event.touch.tmStampUs = sdlEvent.button.timestamp * 1000UL;
                event.touch.lastT = event.touch.tmStampUs;
                event.touch.count = 1;
            }

            return true;
        }

        return false;
    }

    void InputSource::connectOnEvent(sigc::slot<void, InputEvent *> onEvent)
    {
        private_->connectOnEvent(onEvent);
    }
}
