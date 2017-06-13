/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_CYLINDER
#define GRAPEVINE_MATH_CYLINDER

#include "Grapevine/Math/Vector.hpp"

namespace Grapevine
{
    class Cylinder
    {
    public:
        Cylinder(Vector3 const& origin = Vector3(), Vector3 const& direction = Vector3(), float radius = 0.0f);
        Vector3 const& origin() const;
        Vector3 const& direction() const;
        float radius() const;

    private:
        Vector3 origin_;
        Vector3 direction_;
        float radius_;
    };
}

#endif
