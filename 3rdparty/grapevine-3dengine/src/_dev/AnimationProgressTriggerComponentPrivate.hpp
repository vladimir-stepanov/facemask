/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ANIMATIONPROGRESSTRIGGERCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_ANIMATIONPROGRESSTRIGGERCOMPONENTPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AnimationProgressTriggerComponentPrivate: public AbstractComponentPrivateTemplate<AnimationProgressTriggerComponentPrivate>
            {
            public:
                sigc::signal<void> trigger;

                CLASS_UNCOPYABLE(AnimationProgressTriggerComponentPrivate)

                AnimationProgressTriggerComponentPrivate(float triggerValue);
                ~AnimationProgressTriggerComponentPrivate();

            private:
                friend class AnimatorVisitor;
                friend class ListAnimatorVisitor;
                bool triggerEmitted_;
                float value_;
            };
        }
    }
}

#endif
