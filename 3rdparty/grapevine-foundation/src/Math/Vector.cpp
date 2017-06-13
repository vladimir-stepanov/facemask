/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Vector.hpp"
#include <math.h>

// workaround for builds without sincosf (such as the build for Nexus5)
// marked as weak symbol so that builds with sincosf will use the builtin one
#ifndef TSAIKKA
void __attribute__((weak)) sincosf(float x, float *s, float *c)
{
    *s = sin(x);
    *c = cos(x);
}
#endif

namespace Grapevine
{
    namespace Vector
    {
        Vector3 transform(Vector3 const& v, Matrix4 const& m)
        {
            return Vector3(v.x() * m[0][0] + v.y() * m[1][0] + v.z() * m[2][0],
                v.x() * m[0][1] + v.y() * m[1][1] + v.z() * m[2][1],
                v.x() * m[0][2] + v.y() * m[1][2] + v.z() * m[2][2]);
        }
    }
}
