/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_RAY
#define GRAPEVINE_MATH_RAY

#include <Grapevine/Core/Class.hpp>
#include "Grapevine/Math/Vector.hpp"
#include "Grapevine/Math/Plane.hpp"
#include "Grapevine/Math/Sphere.hpp"
#include "Grapevine/Math/Cylinder.hpp"
#include "Grapevine/Math/RayPlaneIntersection.hpp"
#include "Grapevine/Math/RaySphereIntersection.hpp"
#include "Grapevine/Math/RayCylinderIntersection.hpp"

namespace Grapevine
{
    class Ray
    {
    public:
        CLASS_COPYABLE(Ray)

        Ray(Vector3 const& startLocation, Vector3 const& unit);
        Ray();
        RayPlaneIntersection intersection(Plane const& plane) const;
        RaySphereIntersection intersection(Sphere const& sphere) const;
        RayCylinderIntersection intersection(Cylinder const& cylinder) const;
        bool intersectOBB(Vector3 const& AABBMin, Vector3 const& AABBMax, Matrix4 const& AABBTransformation, Ray& rayInAABBSpace) const;
        bool intersectAABB(Vector3 const& AABBMin, Vector3 const& AABBMax) const;
        float distance(Vector3 const& point) const;
        Vector3 closestPoint(Vector3 const& point) const;
        Vector3 origin() const;
        Vector3 unit() const;
        static Ray transform(Grapevine::Ray const& ray, Matrix4 const& worldTransformation);

    private:
        Vector3 origin_;
        Vector3 unit_;
    };
}

#endif
