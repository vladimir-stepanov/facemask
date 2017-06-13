/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Math/Matrix.hpp"
#include <math.h>

namespace Grapevine
{
    namespace Matrix
    {
        Matrix4 translate(float x, float y, float z)
        {
            Matrix4 r;
            r[0][3] = x;
            r[1][3] = y;
            r[2][3] = z;
            return r;
        }

        Matrix4 scale(float x, float y, float z)
        {
            Matrix4 r;
            r[0][0] = x;
            r[1][1] = y;
            r[2][2] = z;
            return r;
        }

        Matrix4 rotate(float angle, float x, float y, float z)
        {
            Vector3 u(x, y, z);
            u.normalize();
            Matrix3 uuT = outer(u, u);

            Matrix3 s;
            s[0][0] = 0;
            s[0][1] = -u.z();
            s[0][2] = u.y();
            s[1][0] = u.z();
            s[1][1] = 0;
            s[1][2] = -u.x();
            s[2][0] = -u.y();
            s[2][1] = u.x();
            s[2][2] = 0;

            Matrix3 i;
            i -= uuT;
            float angleRadians(angle * M_PI / 180.0);
            i *= cos(angleRadians);
            s *= sin(angleRadians);
            i += s;
            Matrix3 m = uuT + i;

            Matrix4 r;
            r[0][0] = m[0][0];
            r[0][1] = m[0][1];
            r[0][2] = m[0][2];
            r[1][0] = m[1][0];
            r[1][1] = m[1][1];
            r[1][2] = m[1][2];
            r[2][0] = m[2][0];
            r[2][1] = m[2][1];
            r[2][2] = m[2][2];

            return r;
        }

        Matrix4 rotateRadians(float angleRadians, Vector3 const& u)
        {
            Matrix3 uuT = outer(u, u);

            Matrix3 s;
            s[0][0] = 0;
            s[0][1] = -u.z();
            s[0][2] = u.y();
            s[1][0] = u.z();
            s[1][1] = 0;
            s[1][2] = -u.x();
            s[2][0] = -u.y();
            s[2][1] = u.x();
            s[2][2] = 0;

            Matrix3 i;
            i -= uuT;
            i *= cos(angleRadians);
            s *= sin(angleRadians);
            i += s;
            Matrix3 m = uuT + i;

            Matrix4 r;
            r[0][0] = m[0][0];
            r[0][1] = m[0][1];
            r[0][2] = m[0][2];
            r[1][0] = m[1][0];
            r[1][1] = m[1][1];
            r[1][2] = m[1][2];
            r[2][0] = m[2][0];
            r[2][1] = m[2][1];
            r[2][2] = m[2][2];

            return r;
        }

        Matrix4 rotate(float angle, Vector3 const& axis)
        {
            return rotate(angle, axis.x(), axis.y(), axis.z());
        }

        Matrix4 frustum(float left, float right, float bottom, float top, float near, float far)
        {
            float twiceNear(2 * near);
            float width(right - left);
            float height(top - bottom);
            float depth(far - near);

            Matrix4 r;
            r[0][0] = twiceNear / width;
            r[0][2] = (right + left) / width;
            r[1][1] = twiceNear / height;
            r[1][2] = (top + bottom) / height;
            r[2][2] = -(far + near) / depth;
            r[2][3] = -(twiceNear * far) / depth;
            r[3][2] = -1;
            r[3][3] = 0;

            return r;
        }

        Matrix4 ortho(float left, float right, float bottom, float top, float near, float far)
        {
            float width = right - left;
            float height = top - bottom;
            float depth = far - near;

            Matrix4 r;
            r[0][0] = 2.0f / width;
            r[1][1] = 2.0f / height;
            r[2][2] = 1.0f / depth;
            r[2][3] = -near / depth;
            r[3][3] = 1.0f;
            return r;
        }

        Matrix4 perspective(float fieldOfViewY, float aspect, float zNear, float zFar)
        {
            // Degrees to radians
            float fovyRadians(fieldOfViewY * M_PI / 180.0);

            // Cotangent(x) = 1/tan(x)
            float f = 1 / tan(fovyRadians / 2);
            float depth(zNear - zFar);

            Matrix4 r;
            r[0][0] = f / aspect;
            r[1][1] = f;
            r[2][2] = (zFar + zNear) / depth;
            r[2][3] = (2 * zFar * zNear) / depth;
            r[3][2] = -1;
            r[3][3] = 0;

            return r;
        }

        Matrix4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX,
            float centerY, float centerZ, float upX, float upY, float upZ)
        {
            Vector3 f(centerX - eyeX, centerY - eyeY, centerZ - eyeZ);
            f.normalize();
            Vector3 up(upX, upY, upZ);
            Vector3 s = Vector3::cross(f, up);
            Vector3 u = Vector3::cross(s, f);
            s.normalize();
            u.normalize();

            Matrix4 r;
            r[0][0] = s.x();
            r[0][1] = s.y();
            r[0][2] = s.z();
            r[1][0] = u.x();
            r[1][1] = u.y();
            r[1][2] = u.z();
            r[2][0] = -f.x();
            r[2][1] = -f.y();
            r[2][2] = -f.z();
            r *= translate(-eyeX, -eyeY, -eyeZ);

            return r;
        }

        Matrix4 lookAt(Vector3 const& eye, Vector3 const& center,
            Vector3 const& up)
        {
            Vector3 f = center - eye;
            f.normalize();
            Vector3 s = Vector3::cross(f, up);
            Vector3 u = Vector3::cross(s, f);
            s.normalize();
            u.normalize();

            Matrix4 r;
            r[0][0] = s.x();
            r[0][1] = s.y();
            r[0][2] = s.z();
            r[1][0] = u.x();
            r[1][1] = u.y();
            r[1][2] = u.z();
            r[2][0] = -f.x();
            r[2][1] = -f.y();
            r[2][2] = -f.z();
            r *= translate(-eye.x(), -eye.y(), -eye.z());

            return r;
        }

        Matrix4 eulerAngles(float x0, float y0, float z0)
        {
            float x(x0 * M_PI / 180.0f);
            float y(y0 * M_PI / 180.0f);
            float z(z0 * M_PI / 180.0f);

            float sa = sin(z); // sin attitude 
            float ca = cos(z); // cos attitude 
            float sb = sin(x); // sin bank
            float cb = cos(x); // cos bank
            float sh = sin(y); // sin head
            float ch = cos(y); // cos head

            Matrix3 s;
            s[0][0] = ch*ca;
            s[0][1] = -ch*sa*cb + sh*sb;
            s[0][2] = ch*sa*sb + sh*cb;
            s[1][0] = sa;
            s[1][1] = ca*cb;
            s[1][2] = -ca*sb;
            s[2][0] = -sh*ca;
            s[2][1] = sh*sa*cb + ch*sb;
            s[2][2] = -sh*sa*sb + ch*cb;

            Matrix4 r;
            r[0][0] = s[0][0];
            r[0][1] = s[0][1];
            r[0][2] = s[0][2];
            r[1][0] = s[1][0];
            r[1][1] = s[1][1];
            r[1][2] = s[1][2];
            r[2][0] = s[2][0];
            r[2][1] = s[2][1];
            r[2][2] = s[2][2];

            return r;
        }

        Matrix4 eulerAngles(Vector3 const& angles)
        {
            return eulerAngles(angles.x(), angles.y(), angles.z());
        }

    }
}
