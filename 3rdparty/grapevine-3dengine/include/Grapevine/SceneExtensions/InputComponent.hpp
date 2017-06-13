/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_INPUTCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_INPUTCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/Math/Ray.hpp>
#include <string>
#include <sigc++/signal.h>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class InputComponent: public Component
            {
            public:
                CLASS_COPYABLE(InputComponent)

                struct StateFlag
                {
                    enum Enum
                    {
                        Stationary = 1,
                        Moving = 2,
                        Pressed = 4,
                        Released = 8,
                        Cancel = 16,
                        Mask = 31
                    };
                };

                struct Event
                {
                    Event():
                        fingerId(0), // TODO
                        ray(Vector3(), Vector3()),
                        flags(0)
                    {
                    }

                    unsigned int fingerId;
                    Ray ray;
                    unsigned int flags;
                };


                InputComponent(ReferencedPointer<Referenced> const&);
                virtual ~InputComponent();
                ReferencedPointer<Referenced> const& userData();

                void connectInput(sigc::slot<void, Event const&, ReferencedPointer<Referenced> const&>);

            private:
                class InputComponentPrivate* componentPrivate_;
            };
        }
    }
}

#endif
