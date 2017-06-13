/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Plane.hpp"

namespace Grapevine
{
    Plane::Plane(Vector3 const& location, Vector3 const& normal):
        normal_(normal.normal()),
        normalLength_(sqrt((normal_.x() * normal_.x()) + (normal_.y() * normal_.y()) + (normal_.z() * normal_.z()))),
        distance_(0.0f)
    {
        // We know that:
        //     1) Normal is (a, b, c), length of normal is 1
        //     2) Plane equation is a*x + b*y + c*z + d = 0
        //
        // We have location (i, j, k) and so we can calculate d like next:
        //     d = -(a*i + b*j + c*k)

        distance_ = -(normal_.x() * location.x() + normal_.y() * location.y() + normal_.z() * location.z());
    }

    Plane::Plane(Vector3 const& normal, float distance):
        normal_(normal.normal()),
        normalLength_(sqrt((normal_.x() * normal_.x()) + (normal_.y() * normal_.y()) + (normal_.z() * normal_.z()))),
        distance_(distance)
    {
    }

    Plane::Plane(Plane const& r):
        normal_(r.normal_),
        normalLength_(r.normalLength_),
        distance_(r.distance_)
    {
    }

    Plane& Plane::operator=(Plane const& r)
    {
        if (this != &r)
        {
            normal_ = r.normal_;
            normalLength_ = r.normalLength_;
            distance_ = r.distance_;
        }
        return *this;
    }

    Vector3 const& Plane::normal() const
    {
        return normal_;
    }

    float Plane::distance() const
    {
        return distance_;
    }

    float Plane::distanceToPoint(const Vector3 & p) const
    {
        float a = fabs((normal_.x() * p.x()) + (normal_.y() * p.y()) + (normal_.z() * p.z()) + distance_);
        return a / normalLength_;
    }

    float Plane::normalDistanceToPoint(const Vector3 & p) const
    {
        float a = (normal_.x() * p.x()) + (normal_.y() * p.y()) + (normal_.z() * p.z()) + distance_;
        return a / normalLength_;
    }
}
