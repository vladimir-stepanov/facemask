/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_ANIMATION
#define GRAPEVINE_ANIMATION_ANIMATION

#include "Grapevine/Animation/Easing.hpp"
#include "Grapevine/Animation/Path.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Animation/AnimationPrivate.hpp>
#include <sigc++/signal.h>

namespace Grapevine
{

    template <typename T>
    class Animation
    {
    public:
        //CLASS_COPYABLE(Animation)

        Animation(float durationMilliseconds, Easing const& easing, Path<T> const& path):
            private_(ReferencedPointer<AnimationPrivate<T> >(new AnimationPrivate<T>(durationMilliseconds, easing, path)))
        {
        }

        Animation(Animation const& r):
            private_(r.private_)
        {
        }

        virtual ~Animation()
        {
        }

        Animation& operator=(Animation const& r)
        {
            if (this != &r)
            {
                private_ = r.private_;
            }
            return *this;
        }

        void start()
        {
            private_.pointer()->start();
        }

        bool done()
        {
            return private_.pointer()->done();
        }

        float location() const
        {
            return private_.pointer()->location();
        }

        float progress() const
        {
            return private_.pointer()->progress();
        }

        T value() const
        {
            return private_.pointer()->value();
        }

    private:
        ReferencedPointer<AnimationPrivate<T> > private_;
    };
}

#endif
