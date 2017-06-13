/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Box.hpp"

namespace Grapevine
{
    Box::Box(Vector3 const& min, Vector3 const& max):
        min_(min),
        max_(max)
    {
    }

    Box::Box(Vector3 const& maxFromOrigo):
        min_(Vector::origo),
        max_(maxFromOrigo)
    {
    }

    Box::Box(Box const& r):
        min_(r.min_),
        max_(r.max_)
    {
    }

    Box& Box::operator=(Box const& r)
    {
        if (this != &r)
        {
            min_ = r.min_;
            max_ = r.max_;
        }
        return *this;
    }

    void Box::setMin(Vector3 const& min)
    {
        min_ = min;
    }

    void Box::setMax(Vector3 const& max)
    {
        max_ = max;
    }

    Vector3 const& Box::min() const
    {
        return min_;
    }

    Vector3 const& Box::max() const
    {
        return max_;
    }

}
