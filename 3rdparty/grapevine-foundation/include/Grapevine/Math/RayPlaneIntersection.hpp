/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_RAYPLANEINTERSECTION
#define GRAPEVINE_MATH_RAYPLANEINTERSECTION

#include "Grapevine/Math/Vector.hpp"

namespace Grapevine
{
    class RayPlaneIntersection
    {
    public:
        struct Type
        {
            enum Enum
            {
                PointIntersection,
                ParallelOnPlane,
                ParallelNoIntersection,
                PointsAwayNoIntersection
            };
        };

        RayPlaneIntersection(Type::Enum type = Type::PointsAwayNoIntersection, Vector3 const& p = Vector3()):
            type_(type),
            p_(p)
        {
        }

        Type::Enum type() const
        {
            return type_;
        }

        Vector3 const& p() const
        {
            return p_;
        }

    private:
        Type::Enum type_;
        Vector3 p_;
    };
}

#endif
