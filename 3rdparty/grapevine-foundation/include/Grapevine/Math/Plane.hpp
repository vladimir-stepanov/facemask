/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_PLANE
#define GRAPEVINE_MATH_PLANE

#include "Grapevine/Math/Vector.hpp"
#include "Grapevine/Core/Class.hpp"

namespace Grapevine
{
    class Plane
    {
    public:
        CLASS_COPYABLE(Plane)

        Plane(Vector3 const& location = Vector3(0, 0, 0), Vector3 const& normal = Vector3(0, 1, 0));
        Plane(Vector3 const& normal, float distance);
        Vector3 const& normal() const;
        float distance() const;
        float distanceToPoint(const Vector3 & point) const;
        float normalDistanceToPoint(const Vector3 & p) const;

    private:
        Vector3 normal_;
        float normalLength_;
        float distance_;
    };
}

#endif
