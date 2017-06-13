/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_LINEARPATH
#define GRAPEVINE_ANIMATION_LINEARPATH

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Animation/Path.hpp>

namespace Grapevine
{

    template <typename T>
    class LinearPathPrivate: public AbstractPathPrivate<T>
    {
    public:
        //CLASS_UNCOPYABLE(LinearPathPrivate)

        LinearPathPrivate(T const& p0, T const& p1):
            AbstractPathPrivate<T>(),
            p0_(p0),
            p1_(p1)
        {
        }

        virtual ~LinearPathPrivate()
        {
        }

        virtual T value(float offset) const
        {
            const float t = offset;
            return t * p1_ + (1.0f - t) * p0_;
        }

    private:
        T p0_;
        T p1_;
    };

    template <typename T>
    class LinearPath: public Path<T>
    {
    public:
        //CLASS_COPYABLE(LinearPath)

        LinearPath(T const& p0, T const& p1):
            Path<T>(new LinearPathPrivate<T>(p0, p1))
        {
        }

        ~LinearPath()
        {
        }
    };
}

#endif
