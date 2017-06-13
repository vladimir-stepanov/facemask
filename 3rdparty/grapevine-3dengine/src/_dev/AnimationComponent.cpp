/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/AnimationComponent.hpp"
#include "Grapevine/SceneExtensions/AnimationComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AnimationComponent::AnimationComponent(Animation const& animation, bool autoRemove):
                Component(new AnimationComponentPrivate(animation, autoRemove))
            {
                animationComponentPrivate_ = dynamic_cast<AnimationComponentPrivate*>(private_.pointer());
                done_ = animationComponentPrivate_->done_;
            }

            AnimationComponent::AnimationComponent(AnimationComponent const &r)
            : Component(r),
              done_(r.done_)
            {
                animationComponentPrivate_ = r.animationComponentPrivate_;
                done_ = animationComponentPrivate_->done_;
            }

            AnimationComponent::~AnimationComponent()
            {
            }

            AnimationComponent &AnimationComponent::operator=(AnimationComponent const &r)
            {
                Component::operator=(r);
                animationComponentPrivate_ = r.animationComponentPrivate_;
                done_ = r.done_;
                return *this;
            }

            void AnimationComponent::start()
            {
                animationComponentPrivate_->start();
            }

            bool AnimationComponent::done()
            {
                return animationComponentPrivate_->done();
            }
        }
    }
}
