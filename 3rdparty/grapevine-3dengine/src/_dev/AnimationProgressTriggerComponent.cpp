/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/AnimationProgressTriggerComponent.hpp"
#include "Grapevine/SceneExtensions/AnimationProgressTriggerComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AnimationProgressTriggerComponent::AnimationProgressTriggerComponent(float value):
                Component(new AnimationProgressTriggerComponentPrivate(value))
            {
                animationProgressTriggerComponentPrivate_ = dynamic_cast<AnimationProgressTriggerComponentPrivate*>(private_.pointer());
                trigger = animationProgressTriggerComponentPrivate_->trigger;
            }

            AnimationProgressTriggerComponent::AnimationProgressTriggerComponent(AnimationProgressTriggerComponent const &r):
                Component(r),
                trigger(r.trigger)
            {
                animationProgressTriggerComponentPrivate_ = r.animationProgressTriggerComponentPrivate_;
            }

            AnimationProgressTriggerComponent::~AnimationProgressTriggerComponent()
            {
            }

            AnimationProgressTriggerComponent &AnimationProgressTriggerComponent::operator=(AnimationProgressTriggerComponent const &r)
            {
                Component::operator=(r);
                animationProgressTriggerComponentPrivate_ = r.animationProgressTriggerComponentPrivate_;
                trigger = r.trigger;
                return *this;
            }
        }
    }
}
