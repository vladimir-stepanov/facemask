/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ANIMATIONPROGRESSTRIGGERCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_ANIMATIONPROGRESSTRIGGERCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/Animation/Animation.hpp>
#include <string>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AnimationProgressTriggerComponent: public Component
            {
            public:
                sigc::signal<void> trigger;

                CLASS_COPYABLE(AnimationProgressTriggerComponent)

                AnimationProgressTriggerComponent(float value);
                virtual ~AnimationProgressTriggerComponent();

            private:
                class AnimationProgressTriggerComponentPrivate* animationProgressTriggerComponentPrivate_;
            };
        }
    }
}

#endif
