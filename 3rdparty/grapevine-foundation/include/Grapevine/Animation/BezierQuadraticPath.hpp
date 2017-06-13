/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_BEZIERQUADRATICPATH
#define GRAPEVINE_ANIMATION_BEZIERQUADRATICPATH

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Util.hpp>
#include <Grapevine/Animation/Path.hpp>
#include <Grapevine/Animation/ControlPoint.hpp>

namespace Grapevine
{

    template <typename T>
    class BezierQuadraticPathPrivate: public AbstractPathPrivate<T>
    {
    public:
        //CLASS_UNCOPYABLE(BezierQuadraticPathPrivate)

        BezierQuadraticPathPrivate(T const& p0, T const& p1, T const& p2):
            AbstractPathPrivate<T>(),
            p0_(p0),
            p1_(p1),
            p2_(p2)
        {
        }

        virtual ~BezierQuadraticPathPrivate()
        {
        }

        virtual T value(float offset) const
        {
            float t = Util::clamp(offset, 0.0f, 1.0f);

            // B(t) = (1 - t) * [(1 - t) * P0 + t * P1] + t * [(1 - t) * P1 + t * P2]   |  t = [0, 1]
            // B(t) = [(1 - t)^2 * P0] + [2 * (1 - t) * t * P1] + [t^2 * P1]
            float oneMinusT = (1 - t);
            float twoTimesOneMinusT = 2.0f * oneMinusT;
            return (oneMinusT * oneMinusT * p0_) + (twoTimesOneMinusT * t * p1_) + (t * t * p2_);
        }

    private:
        T p0_;
        T p1_;
        T p2_;
    };

    template <typename T>
    class BezierQuadraticPath: public Path<T>
    {
    public:
        //CLASS_COPYABLE(BezierQuadraticPath)

        BezierQuadraticPath(T const& p0, T const& p1, T const& p2):
            Path<T>(new BezierQuadraticPathPrivate<T>(p0, p1, p2))
        {
        }

        ~BezierQuadraticPath()
        {
        }
    };
}

#endif
