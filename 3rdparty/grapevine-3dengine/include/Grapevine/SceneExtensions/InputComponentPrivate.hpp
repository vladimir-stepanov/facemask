/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_INPUTCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_INPUTCOMPONENTPRIVATE

#include "Grapevine/SceneExtensions/InputComponent.hpp"
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>
#include <sigc++/signal.h>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class InputComponentPrivate: public AbstractComponentPrivateTemplate<InputComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(InputComponentPrivate)

                sigc::signal<void, InputComponent::Event const&, ReferencedPointer<Referenced> const&> input;

                InputComponentPrivate(ReferencedPointer<Referenced> const& userData);
                ~InputComponentPrivate();
                ReferencedPointer<Referenced> const& userData();

            private:
                ReferencedPointer<Referenced> userData_;
            };
        }
    }
}

#endif
