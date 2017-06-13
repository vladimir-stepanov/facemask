/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_MATH
#define GRAPEVINE_MATH_MATH

namespace Grapevine
{
    namespace Math
    {
        static const float Pi = 3.14159265358979323846;

        static bool equalWithTolerance(float a, float b, float tolerance)
        {
            float tmp = a - b;
            return (tmp >= 0 ? tmp : -tmp) < tolerance;
        }
    }
}

#endif
