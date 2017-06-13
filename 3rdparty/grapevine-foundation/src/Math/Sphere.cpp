/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Sphere.hpp"

namespace Grapevine
{
    Sphere::Sphere(Vector3 const& center, float radius):
        center_(center),
        radius_(radius)
    {
    }

    Sphere::Sphere(Sphere const& r):
        center_(r.center_),
        radius_(r.radius_)
    {
    }

    Sphere& Sphere::operator=(Sphere const& r)
    {
        if (this != &r) {
            center_ = r.center_;
            radius_ = r.radius_;
        }
        return *this;
    }

    Vector3 const& Sphere::center() const
    {
        return center_;
    }

    void Sphere::setCenter(Vector3 const& center)
    {
        center_ = center;
    }

    float Sphere::radius() const
    {
        return radius_;
    }

    void Sphere::setRadius(float radius)
    {
        radius_ = radius;
    }


}
