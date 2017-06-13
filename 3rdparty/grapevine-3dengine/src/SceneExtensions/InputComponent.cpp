/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/InputComponent.hpp"
#include "Grapevine/SceneExtensions/InputComponentPrivate.hpp"

#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Core/Referenced.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            InputComponent::InputComponent(ReferencedPointer<Referenced> const& userData):
                Component(new InputComponentPrivate(userData))
            {
                componentPrivate_ = dynamic_cast<InputComponentPrivate*>(private_.pointer());
            }

            InputComponent::InputComponent(InputComponent const &r)
            : Component(r)
            {
                componentPrivate_ = r.componentPrivate_;
            }

            InputComponent::~InputComponent()
            {
            }

            InputComponent &InputComponent::operator=(InputComponent const &r)
            {
                componentPrivate_ = r.componentPrivate_;
                Component::operator=(r);
                return *this;
            }

            void InputComponent::connectInput(sigc::slot<void, Event const&, ReferencedPointer<Referenced> const&> slot)
            {
                componentPrivate_->input.connect(slot);
            }

            ReferencedPointer<Referenced> const& InputComponent::userData()
            {
                return componentPrivate_->userData();
            }
        }
    }
}
