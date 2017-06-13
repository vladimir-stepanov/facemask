/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/AnimationComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AnimationComponentPrivate::AnimationComponentPrivate(Animation const& animation, bool autoRemove):
                AbstractComponentPrivateTemplate<AnimationComponentPrivate>("AnimationComponent"),
                animation_(animation),
                doneEmitted_(false),
                autoRemove_(autoRemove)
            {
            }

            AnimationComponentPrivate::~AnimationComponentPrivate()
            {
            }

            void AnimationComponentPrivate::start()
            {
               animation_.start();
               doneEmitted_ = false;
            }

            bool AnimationComponentPrivate::done()
            {
                return animation_.done() && doneEmitted_;
            }

            void AnimationComponentPrivate::emitDone()
            {
                if(animation_.done() && !doneEmitted_)
                {
                    done_.emit();
                    doneEmitted_ = true;
                }
            }

            Vector3 AnimationComponentPrivate::translation() const
            {
                return animation_.translation();
            }

            Matrix4 AnimationComponentPrivate::rotation() const
            {
                return animation_.rotation();
            }

            Vector3 AnimationComponentPrivate::scale() const
            {
                return animation_.scale();
            }

            float AnimationComponentPrivate::opacity() const
            {
                return animation_.opacity();
            }

            bool AnimationComponentPrivate::autoRemove() const
            {
                return autoRemove_;
            }
        }
    }
}
