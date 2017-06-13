/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_MATRIX
#define GRAPEVINE_MATH_MATRIX

#include "Grapevine/Math/Vector.hpp"
#include "Grapevine/Math/ProxyMatrix.hpp"
#include "Grapevine/Math/Matrix2Template.hpp"
#include "Grapevine/Math/Matrix3Template.hpp"
#include "Grapevine/Math/Matrix4Template.hpp"
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace Grapevine
{
    template<typename T> class Vector3Template;
    typedef Vector3Template<float> Vector3;

    typedef Matrix2Template<float> Matrix2;
    typedef Matrix3Template<float> Matrix3;
    typedef Matrix4Template<float> Matrix4;

    typedef Matrix2Template<double> Matrix2d;
    typedef Matrix3Template<double> Matrix3d;
    typedef Matrix4Template<double> Matrix4d;

    typedef Matrix2Template<int> Matrix2i;
    typedef Matrix3Template<int> Matrix3i;
    typedef Matrix4Template<int> Matrix4i;

    typedef Matrix2Template<unsigned int> Matrix2u;
    typedef Matrix3Template<unsigned int> Matrix3u;
    typedef Matrix4Template<unsigned int> Matrix4u;

    typedef Matrix2Template<bool> Matrix2b;
    typedef Matrix3Template<bool> Matrix3b;
    typedef Matrix4Template<bool> Matrix4b;

    namespace Matrix
    {
        Matrix4 translate(float x, float y, float z);
        Matrix4 scale(float x, float y, float z);
        Matrix4 rotate(float angle, float x, float y, float z);
        Matrix4 rotate(float angle, Vector3 const& axis);
        Matrix4 rotateRadians(float angleRadians, Vector3 const& unitAxis);
        Matrix4 frustum(float left, float right, float bottom, float top, float near, float far);
        Matrix4 ortho(float left, float right, float bottom, float top, float near, float far);
        Matrix4 perspective(float fieldOfViewY, float aspect, float zNear, float zFar);
        Matrix4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);
        Matrix4 lookAt(Vector3 const& eye, Vector3 const& center, Vector3 const& up);
        Matrix4 eulerAngles(float x, float y, float z);
        Matrix4 eulerAngles(Vector3 const& angles);
    }
}

#endif
