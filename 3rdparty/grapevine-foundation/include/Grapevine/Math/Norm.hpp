/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_NORM
#define GRAPEVINE_MATH_NORM

#include "Grapevine/Math/Vector.hpp"

namespace Grapevine
{
    template<typename T>
    T L2Norm(const Vector2Template<T> &vec)
    {
        return vec.length();
    }

    template<typename T>
    T L2Norm(const Vector3Template<T> &vec)
    {
        return vec.length();
    }

    template<typename T>
    T L2Norm(const Vector4Template<T> &vec)
    {
        return vec.length();
    }

    template<typename T>
    T L2Norm(T value)
    {
        return std::abs(value);
    }

}

#endif
