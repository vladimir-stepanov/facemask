/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_VECTOR
#define GRAPEVINE_MATH_VECTOR

#include "Grapevine/Math/Vector2Template.hpp"
#include "Grapevine/Math/Vector3Template.hpp"
#include "Grapevine/Math/Vector4Template.hpp"
#include "Grapevine/Math/Matrix.hpp"

namespace Grapevine
{
    template<typename T> class Matrix4Template;
    typedef Matrix4Template<float> Matrix4;

    typedef Vector2Template<float> Vector2;
    typedef Vector3Template<float> Vector3;
    typedef Vector4Template<float> Vector4;

    typedef Vector2Template<double> Vector2d;
    typedef Vector3Template<double> Vector3d;
    typedef Vector4Template<double> Vector4d;

    typedef Vector2Template<int> Vector2i;
    typedef Vector3Template<int> Vector3i;
    typedef Vector4Template<int> Vector4i;

    typedef Vector2Template<unsigned int> Vector2u;
    typedef Vector3Template<unsigned int> Vector3u;
    typedef Vector4Template<unsigned int> Vector4u;

    typedef Vector2Template<bool> Vector2b;
    typedef Vector3Template<bool> Vector3b;
    typedef Vector4Template<bool> Vector4b;

    namespace Vector
    {
        static const Vector3 xAxis = Vector3(1.0f, 0.0f, 0.0f);
        static const Vector3 yAxis = Vector3(0.0f, 1.0f, 0.0f);
        static const Vector3 zAxis = Vector3(0.0f, 0.0f, 1.0f);
        static const Vector3 minusXAxis = Vector3(-1.0f, 0.0f, 0.0f);
        static const Vector3 minusYAxis = Vector3(0.0f, -1.0f, 0.0f);
        static const Vector3 minusZAxis = Vector3(0.0f, 0.0f, -1.0f);
        static const Vector3 origo = Vector3(0.0f, 0.0f, 0.0f);

        Vector3 transform(Vector3 const& vector, Matrix4 const& matrix);
    }
}

#endif
