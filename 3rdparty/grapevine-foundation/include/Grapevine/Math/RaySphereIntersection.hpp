/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_RAYSPHEREINTERSECTION
#define GRAPEVINE_MATH_RAYSPHEREINTERSECTION

#include "Grapevine/Math/Vector.hpp"

namespace Grapevine
{
    class RaySphereIntersection
    {
    public:
        struct Type {
            enum Enum {
                PointIntersection,
                TwoPointIntersection,
                NoIntersection
            };
        };

        RaySphereIntersection(Type::Enum type, Vector3 const& p0 = Vector3(),
            Vector3 const& p1 = Vector3()):
            type_(type),
            p0_(p0),
            p1_(p1)
        {
        }

        Type::Enum type() const
        {
            return type_;
        }

        Vector3 const& p0() const
        {
            return p0_;
        }

        Vector3 const& p1() const
        {
            return p1_;
        }

    private:
        Type::Enum type_;
        Vector3 p0_;
        Vector3 p1_;
    };
}

#endif
