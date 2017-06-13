/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/AnimationProgressTriggerComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AnimationProgressTriggerComponentPrivate::AnimationProgressTriggerComponentPrivate(float value):
                AbstractComponentPrivateTemplate<AnimationProgressTriggerComponentPrivate>("AnimationProgressTriggerComponent"),
                triggerEmitted_(false),
                value_(value)
            {
            }

            AnimationProgressTriggerComponentPrivate::~AnimationProgressTriggerComponentPrivate()
            {
            }
        }
    }
}
