/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Ray.hpp"
#include "Grapevine/Math/RayPlaneIntersection.hpp"
#include "Grapevine/Math/RaySphereIntersection.hpp"
#include <Grapevine/Core/Log.hpp>
#include "Grapevine/Math/Math.hpp"
#include <float.h>
#include <math.h>

namespace Grapevine
{
    float squaredDistance(Vector3 const& point, Vector3 const& linePoint, Vector3 const& lineUnit, float& t)
    {
        Vector3 diff = point - linePoint;
        t = Vector3::dot(lineUnit, diff);
        diff -= lineUnit * t;
        return diff.squaredLength();
    }

    Ray::Ray(Vector3 const& startLocation, Vector3 const& unit):
        origin_(startLocation),
        unit_(unit)
    {
    }

    Ray::Ray(const Ray& r):
        origin_(r.origin_),
        unit_(r.unit_)
    {
    }

    Ray::Ray():
        origin_(Vector3(0,0,0)),
        unit_(Vector3(0,0,0))
    {

    }

    Ray& Ray::operator=(const Ray& r)
    {
        if (this != &r)
        {
            origin_ = r.origin_;
            unit_ = r.unit_;
        }
        return *this;
    }

    RayPlaneIntersection Ray::intersection(Plane const& plane) const
    {
        // We know that:
        //      1) Intersection point (X, Y, Z) can be presented like next:
        //          X = rayOrigin.x + rayUnit.x*t
        //          Y = rayOrigin.y + rayUnit.y*t
        //          Z = rayOrigin.z + rayUnit.z*t
        //
        //      2) Plane equation is:
        //          a*x + b*y + c*z + d = 0
        //
        // That is why t can be calculated like next:
        //          t = -(a*rayOrigin.x + b*rayOrigin.y + c*rayOrigin.z + d) / (A*rayUnit.x + B*rayUnit.y + C*rayUnit.z)
        //          t = -(planeNormal·rayOrigin + d) / (planeNormal·rayUnit)
        //
        // First we commute denominator. A ray parallel to the vector will return (n·dir)=0,
        // because the dot product of perpendicular vectors equals zero.

        float denominator = Vector3::dot(plane.normal(), unit_);
        float numerator = -(Vector3::dot(plane.normal(), origin_) + plane.distance());
        RayPlaneIntersection::Type::Enum type = RayPlaneIntersection::Type::PointsAwayNoIntersection;

        if (denominator == 0.0f)
        {
            if (numerator == 0.0f)
            {
                type = RayPlaneIntersection::Type::ParallelOnPlane;
            }
            else
            {
                type = RayPlaneIntersection::Type::ParallelNoIntersection;
            }
        }
        else
        {
            float t = numerator / denominator;
            if (t < 0.0f)
            {
                type = RayPlaneIntersection::Type::PointsAwayNoIntersection;
            }
            else
            {
                type = RayPlaneIntersection::Type::PointIntersection;
                return RayPlaneIntersection(type, origin_ + unit_ * t);
            }
        }
        return RayPlaneIntersection(type);
    }

    RaySphereIntersection Ray::intersection(Sphere const& sphere) const
    {
        // Point p lies on sphere if:
        // dot((p - c), (p - c)) = r^2 | c = sphere center, r = sphere radius
        // ray(t) = o + t*d, t >= 0 || o = ray origin, d = ray direction
        //
        // Setting the ray(t) equal to p:
        // dot((o + t*d - sphere.center), (o + t*d - sphere.center)) = spere.r^2
        //
        // Opening equation:
        // dot(d, d) * t^2 + dot(2 * (o - c), d * t) + dot(o - c, o - c) - r^2 = 0
        // or,
        // At2 + Bt + C = 0 where:
        // A = dot(d, d)
        // B = dot(2 * (o - c), d)
        // C = dot(o - c, o - c) - r^2
        //
        // NOTE: t has to be positive because ray goes to only one direction from
        // origin.

        float a = Vector3::dot(unit_, unit_);
        Vector3 oMinusC = origin_ - sphere.center();
        float b = Vector3::dot(oMinusC * 2.0f, unit_);
        float c = Vector3::dot(oMinusC, oMinusC) - sphere.radius() * sphere.radius();

        // discriminant (b^2 − 4*a*c) < 0
        float discriminant = b * b - a * c * 4.0f;

        // no intersection if discriminant < 0
        if (discriminant < 0.0f)
        {
            return RaySphereIntersection(RaySphereIntersection::Type::NoIntersection);
        }
        else if (discriminant == 0.0f)
        { // one if sphere tangent
            float t0 = -b / (2.0f * a);
            if (t0 < 0.0f)
            {
                return RaySphereIntersection(RaySphereIntersection::Type::NoIntersection);
            }
            else
            {
                return RaySphereIntersection(RaySphereIntersection::Type::PointIntersection, origin_ + t0 * unit_);
            }
        }
        else
        { // zero to two otherwise
            float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
            float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
            if (t0 < 0.0f && t1 < 0.0)
            {
                return RaySphereIntersection(RaySphereIntersection::Type::NoIntersection);
            }
            else if (t0 >= 0.0f && t1 < 0.0f)
            {
                return RaySphereIntersection(
                    RaySphereIntersection::Type::PointIntersection,
                    origin_ + t0 * unit_);
            }
            else if (t0 < 0.0f && t1 >= 0.0f)
            {
                return RaySphereIntersection(
                    RaySphereIntersection::Type::PointIntersection,
                    origin_ + t1 * unit_);
            }
            else
            {
                // Closer is p0 in return
                if (t1 < t0)
                {
                    float tmp = t0;
                    t0 = t1;
                    t1 = tmp;
                }
                return RaySphereIntersection(
                    RaySphereIntersection::Type::TwoPointIntersection,
                    origin_ + t0 * unit_,
                    origin_ + t1 * unit_);
            }
        }
    }

    RayCylinderIntersection Ray::intersection(Cylinder const& cylinder) const
    {
        // Ray : P(t) = O + V * t
        // Cylinder [O, D, r].
        // point Q on cylinder if ((Q - O) x D)^2 = r^2
        //
        // Cylinder [A, B, r].
        // Point P on infinite cylinder if ((P - A) x (B - A))^2 = r^2 * (B - A)^2
        // expand : ((O - A) x (B - A) + t * (V x (B - A)))^2 = r^2 * (B - A)^2
        // equation in the form (X + t * Y)^2 = d, where:
        //  X = (O - A) x (B - A)
        //  Y = V x (B - A)
        //  d = r^2 * (B - A)^2
        //
        // expand the equation:
        // t^2 * (Y . Y) + t * (2 * (X . Y)) + (X . X) - d = 0
        //
        // second order equation in the form: a*t^2 + b*t + c = 0, where:
        // a = (Y . Y)
        // b = 2 * (X . Y)
        // c = (X . X) - d

        Vector3 A = cylinder.origin();
        Vector3 B = cylinder.direction();
        float r = cylinder.radius();
        Vector3 O = origin_;
        Vector3 V = unit_;

        Vector3 AB = (B - A);
        Vector3 AO = (O - A);
        Vector3 AOxAB = Vector3::cross(AO, AB);
        Vector3 VxAB  = Vector3::cross(V, AB);
        float ab2 = Vector3::dot(AB, AB);
        float a = Vector3::dot(VxAB, VxAB);
        float b = 2.0f * Vector3::dot(VxAB, AOxAB);
        float c = Vector3::dot(AOxAB, AOxAB) - (r*r * ab2);

        // solve second order equation : a*t^2 + b*t + c = 0
        // discriminant (b^2 − 4*a*c) < 0
        float discriminant = b * b - a * c * 4.0f;

        // no intersection if discriminant < 0
        if (discriminant < 0.0f)
        {
            return RayCylinderIntersection(RayCylinderIntersection::Type::NoIntersection);
        }
        else if (discriminant == 0.0f)
        { // one if cylinder tangent
            float t0 = -b / (2.0f * a);
            if (t0 < 0.0f) // ray points away
            {
                return RayCylinderIntersection(RayCylinderIntersection::Type::NoIntersection);
            }
            else
            {
                return RayCylinderIntersection(RayCylinderIntersection::Type::OnePointIntersection, origin_ + t0 * unit_);
            }
        }
        else
        { // zero to two otherwise
            float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
            float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
            if (t0 < 0.0f && t1 < 0.0) // ray points away
            {
                return RayCylinderIntersection(RayCylinderIntersection::Type::NoIntersection);
            }
            else if (t0 >= 0.0f && t1 < 0.0f)
            {
                return RayCylinderIntersection(
                    RayCylinderIntersection::Type::OnePointIntersection,
                    origin_ + t0 * unit_);
            }
            else if (t0 < 0.0f && t1 >= 0.0f)
            {
                return RayCylinderIntersection(
                    RayCylinderIntersection::Type::OnePointIntersection,
                    origin_ + t1 * unit_);
            }
            else
            {
                // Closer is p0 in return
                if (t1 < t0)
                {
                    float tmp = t0;
                    t0 = t1;
                    t1 = tmp;
                }
                return RayCylinderIntersection(
                    RayCylinderIntersection::Type::TwoPointIntersection,
                    origin_ + t0 * unit_,
                    origin_ + t1 * unit_);
            }
        }
    }

    bool Ray::intersectOBB(Vector3 const& AABBMin, Vector3 const& AABBMax, Matrix4 const& AABBtransformation, Ray& rayInAABBSpace) const
    {
        Matrix4 inverse(AABBtransformation);
        try {
            inverse.inverse();

            const Vector4 homogenousOrigin(origin_.x(), origin_.y(), origin_.z(), 1.f);
            const Vector4 homogenousUnit(unit_.x(), unit_.y(), unit_.z(), 0.f);
            const Vector4 transformedOrigin(inverse * homogenousOrigin);
            const Vector4 transformedUnit(inverse * homogenousUnit);

            rayInAABBSpace =
                Ray(
                    Vector3
                    (
                        transformedOrigin.x(),
                        transformedOrigin.y(),
                        transformedOrigin.z()
                    ),
                    Vector3
                    (
                        transformedUnit.x(),
                        transformedUnit.y(),
                        transformedUnit.z()
                    )
                );
        } catch (const std::exception &) {
            return false;
        }

        return rayInAABBSpace.intersectAABB(AABBMin, AABBMax);
    }

    bool Ray::intersectAABB(Vector3 const& AABBMin, Vector3 const& AABBMax) const
    {
        // tNear tracks distance to enter the AABB.
        // tFar tracks the distance to exit the AABB.
        float tNear = 0.f; // for future: if line instead of ray, put -FLT_MIN here
        float tFar = FLT_MAX;

        // Test x, y and z planes

        // x plane
        if(!Math::equalWithTolerance(unit_.x(), 0.0f, 1e-4f))
        {
            float reciprocal = 1.0f / unit_.x();
            float t1 = (AABBMin.x() - origin_.x()) * reciprocal;
            float t2 = (AABBMax.x() - origin_.x()) * reciprocal;

            if(t1 < t2)
            {
                tNear = fmax(t1, tNear), tFar = fmin(t2, tFar);
            }
            else // Swap t1 and t2.
            {
                tNear = fmax(t2, tNear), tFar = fmin(t1, tFar);
            }

            if(tNear > tFar)
            {
                // Box is missed since we "exit" before entering it.
                return false;
            }
        }
        else if(origin_.x() < AABBMin.x() || origin_.x() > AABBMax.x())
        {
            // The ray can't possibly enter the box, abort.
            return false;
        }

        // y plane
        if(!Math::equalWithTolerance(unit_.y(), 0.0f, 1e-4f))
        {
            float reciprocal = 1.0f / unit_.y();
            float t1 = (AABBMin.y() - origin_.y()) * reciprocal;
            float t2 = (AABBMax.y() - origin_.y()) * reciprocal;

            if(t1 < t2)
            {
                tNear = fmax(t1, tNear), tFar = fmin(t2, tFar);
            }
            else // Swap t1 and t2.
            {
                tNear = fmax(t2, tNear), tFar = fmin(t1, tFar);
            }

            if(tNear > tFar)
            {
                // Box is missed since we "exit" before entering it.
                return false;
            }
        }
        else if(origin_.y() < AABBMin.y() || origin_.y() > AABBMax.y())
        {
            return false; // The ray can't possibly enter the box, abort.
        }

        // z plane
        if(!Math::equalWithTolerance(unit_.z(), 0.0f, 1e-4f))
        {
            float reciprocal = 1.0f / unit_.z();
            float t1 = (AABBMin.z() - origin_.z()) * reciprocal;
            float t2 = (AABBMax.z() - origin_.z()) * reciprocal;

            if (t1 < t2)
            {
                tNear = fmax(t1, tNear), tFar = fmin(t2, tFar);
            }
            else // Swap t1 and t2.
            {
                tNear = fmax(t2, tNear), tFar = fmin(t1, tFar);
            }
        }
        else if(origin_.z() < AABBMin.z() || origin_.z() > AABBMax.z())
        {
            // The ray can't possibly enter the box, abort.
            return false;
        }

        // If we enter the box before exiting it (or exit and enter in same point)
        // -> we were inside the AABB
        // -> intersection happened!
        return tNear <= tFar;
    }

    float Ray::distance(Vector3 const& point) const
    {
        float t = 0.0f;
        float r = squaredDistance(point, origin_, unit_, t);

        if (t < 0.0f)
        {
            Vector3 diff = point - origin_;
            r = diff.length();
        }
        else
        {
            r = sqrt(r);
        }

        return r;
    }

    Vector3 Ray::closestPoint(Vector3 const& point) const
    {
        Vector3 a = point - origin_;

        // Calculate angle
        // s = dot(a, b) / unit(a)
        //
        // if s > 0, 0 < angle <= 90 and closes point is on ray
        // if s < 0, 0 < angle <= 180 and closest point is origin
        float s = Vector3::dot(a, unit_);
        if (s >= 0)
        {
            return origin_ + unit_ * s;
        }
        return origin_;
    }

    Vector3 Ray::origin() const
    {
        return origin_;
    }

    Vector3 Ray::unit() const
    {
        return unit_;
    }

    Ray Ray::transform(Ray const& ray, Matrix4 const& worldTransformation)
    {
        Matrix4 inverseWorldTransform = worldTransformation;
        inverseWorldTransform.inverse();

        Vector3 const& origin = ray.origin();
        Vector3 const& unit = ray.unit();
        Vector4 const& homogenousOrigin = Vector4(origin.x(), origin.y(), origin.z(), 1.f);
        Vector4 const& homogenousUnit = Vector4(unit.x(), unit.y(), unit.z(), 0.f);
        Vector4 const& transformedOrigin = inverseWorldTransform * homogenousOrigin;
        Vector4 const& transformedUnit = inverseWorldTransform * homogenousUnit;

        return Ray(Vector3(transformedOrigin.x(), transformedOrigin.y(), transformedOrigin.z()),
            Vector3(transformedUnit.x(),   transformedUnit.y(),   transformedUnit.z()));
    }
}
