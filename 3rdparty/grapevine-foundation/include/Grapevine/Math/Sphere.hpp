/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_SPHERE
#define GRAPEVINE_MATH_SPHERE

#include "Grapevine/Math/Vector.hpp"
#include "Grapevine/Core/Class.hpp"

namespace Grapevine
{
    class Sphere
    {
    public:
        CLASS_COPYABLE(Sphere)

        Sphere(Vector3 const& center = Vector3(), float radius = 0.0f);
        Vector3 const& center() const;
        void setCenter(Vector3 const& center);
        float radius() const;
        void setRadius(float radius);

    private:
        Vector3 center_;
        float radius_;
    };
}

#endif
