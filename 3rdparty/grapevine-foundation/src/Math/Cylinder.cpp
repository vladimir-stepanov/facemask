/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Cylinder.hpp"

namespace Grapevine
{
    Cylinder::Cylinder(Vector3 const& origin, Vector3 const& direction, float radius):
        origin_(origin),
        direction_(direction),
        radius_(radius)
    {
    }

    Vector3 const& Cylinder::origin() const
    {
        return origin_;
    }

    Vector3 const& Cylinder::direction() const
    {
        return direction_;
    }

    float Cylinder::radius() const
    {
        return radius_;
    }
}
