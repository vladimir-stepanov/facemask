/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_ANIMATIONPRIVATE
#define GRAPEVINE_ANIMATION_ANIMATIONPRIVATE

#include "Grapevine/Animation/AnimationManager.hpp"
#include "Grapevine/Animation/Easing.hpp"
#include "Grapevine/Animation/Path.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include "Grapevine/Core/Timer.hpp"
#include <sigc++/signal.h>

namespace Grapevine
{
    template <typename T>
    class AnimationPrivate: public Referenced
    {
    public:
        //CLASS_UNCOPYABLE(AnimationPrivate)

        AnimationPrivate(float durationMilliseconds, Easing const& easing, Path<T> const& path):
            started_(false),
            startTimestamp_(0.0f),
            durationMilliseconds_(durationMilliseconds),
            easing_(easing),
            path_(path)
        {
        }

        virtual ~AnimationPrivate()
        {
        }

        void start()
        {
            started_ = true;
            startTimestamp_ = (float)AnimationManager::frameTimestampMilliseconds();
        }

        bool done()
        {
            return animatedMilliseconds() >= durationMilliseconds();
        }

        float durationMilliseconds() const
        {
            return durationMilliseconds_;
        }

        float animatedMilliseconds() const
        {
            float r = 0.0f;
            if (started_ == true)
            {
                r = (float)AnimationManager::frameTimestampMilliseconds() - startTimestamp_;
                if (r > durationMilliseconds_)
                {
                    r = durationMilliseconds_;
                }
            }
            return r;
        }

        float location() const
        {
            return animatedMilliseconds() / durationMilliseconds_;
        }

        float progress() const
        {
            float r = animatedMilliseconds() / durationMilliseconds_;
            if (r > 1.0f)
            {
                r = 1.0f;
            }
            return r;
        }

        T value() const
        {
            float easedTime = easing_.offset(location());
            return path_.value(easedTime);
        }

    private:
        bool started_;
        float startTimestamp_;
        float durationMilliseconds_;
        Easing easing_;
        Path<T> path_;
    };

}

#endif
