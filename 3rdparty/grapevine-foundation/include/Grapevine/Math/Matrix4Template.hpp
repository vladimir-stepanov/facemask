/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_MATRIX4TEMPLATE
#define GRAPEVINE_MATH_MATRIX4TEMPLATE

#include "Grapevine/Math/Quaternion.hpp"
#include "Grapevine/Math/Matrix3Template.hpp"
#include "Grapevine/Math/Vector3Template.hpp"
#include "Grapevine/Math/Vector4Template.hpp"

#include <Grapevine/Core/Log.hpp>
#include <stdexcept>

namespace Grapevine
{
    template<typename T> class Matrix4Template
    {
    public:
        // CLASS_COPYABLE

        Matrix4Template()
        {
            setIdentity();
        }

        Matrix4Template(const Matrix3Template<T>& m)
        {
            m_[0] = m.get(0);
            m_[1] = m.get(1);
            m_[2] = m.get(2);
            m_[3] = 0;
            m_[4] = m.get(3);
            m_[5] = m.get(4);
            m_[6] = m.get(5);
            m_[7] = 0;
            m_[8] = m.get(6);
            m_[9] = m.get(7);
            m_[10] = m.get(8);
            m_[11] = 0;
            m_[12] = 0;
            m_[13] = 0;
            m_[14] = 0;
            m_[15] = 1;
        }

        Matrix4Template(const T m[])
        {
            m_[0] = m[0];
            m_[1] = m[1];
            m_[2] = m[2];
            m_[3] = m[3];
            m_[4] = m[4];
            m_[5] = m[5];
            m_[6] = m[6];
            m_[7] = m[7];
            m_[8] = m[8];
            m_[9] = m[9];
            m_[10] = m[10];
            m_[11] = m[11];
            m_[12] = m[12];
            m_[13] = m[13];
            m_[14] = m[14];
            m_[15] = m[15];
        }

        Vector3Template<T> transform(Vector3Template<T> const& v) const
        {
            Vector3Template<T> r;
            float x = v.x();
            float y = v.y();
            float z = v.z();

            r.x(m_[0]*x + m_[4]*y + m_[8]*z + m_[12]); // v.w is 1.0f so w does not need *1.0f in multiplication
            r.y(m_[1]*x + m_[5]*y + m_[9]*z + m_[13]);
            r.z(m_[2]*x + m_[6]*y + m_[10]*z + m_[14]);

            return r;
        }

        void set(unsigned int r, unsigned int c, T value)
        {
            m_[c * 4 + r] = value;
        }

        void set(unsigned int index, T value)
        {
            m_[index] = value;
        }

        T get(unsigned int index) const
        {
            return m_[index];
        }

        const T* data() const
        {
            return &(m_[0]);
        }

        void setIdentity()
        {
            m_[0] = 1;
            m_[1] = 0;
            m_[2] = 0;
            m_[3] = 0;
            m_[4] = 0;
            m_[5] = 1;
            m_[6] = 0;
            m_[7] = 0;
            m_[8] = 0;
            m_[9] = 0;
            m_[10] = 1;
            m_[11] = 0;
            m_[12] = 0;
            m_[13] = 0;
            m_[14] = 0;
            m_[15] = 1;
        }

        Matrix4Template& transpose()
        {
            T tmp = m_[1];
            m_[1] = m_[4];
            m_[4] = tmp;

            tmp = m_[2];
            m_[2] = m_[8];
            m_[8] = tmp;

            tmp = m_[3];
            m_[3] = m_[12];
            m_[12] = tmp;

            tmp = m_[6];
            m_[6] = m_[9];
            m_[9] = tmp;

            tmp = m_[7];
            m_[7] = m_[13];
            m_[13] = tmp;

            tmp = m_[11];
            m_[11] = m_[14];
            m_[14] = tmp;

            return *this;
        }

        T determinant() const
        {
            Matrix3Template<T> sub0(m_[5], m_[6], m_[7], m_[9], m_[10], m_[11], m_[13], m_[14], m_[15]);
            Matrix3Template<T> sub4(m_[1], m_[2], m_[3], m_[9], m_[10], m_[11], m_[13], m_[14], m_[15]);
            Matrix3Template<T> sub8(m_[1], m_[2], m_[3], m_[5], m_[6], m_[7], m_[13], m_[14], m_[15]);
            Matrix3Template<T> sub12(m_[1], m_[2], m_[3], m_[5], m_[6], m_[7], m_[9], m_[10], m_[11]);
            return (m_[0] * sub0.determinant()) - (m_[4] * sub4.determinant()) +
                (m_[8] * sub8.determinant()) - (m_[12] * sub12.determinant());
        }

        Matrix4Template& inverse() throw(std::runtime_error)
        {
            T d(determinant());
            if (d == static_cast<T>(0))
            {
                throw std::runtime_error("Matrix is noninvertible!");
            }
            Matrix3Template<T> sub0(m_[5], m_[6], m_[7], m_[9], m_[10], m_[11], m_[13], m_[14], m_[15]);
            Matrix3Template<T> sub1(m_[1], m_[2], m_[3], m_[13], m_[14], m_[15], m_[9], m_[10], m_[11]);
            Matrix3Template<T> sub2(m_[1], m_[2], m_[3], m_[5], m_[6], m_[7], m_[13], m_[14], m_[15]);
            Matrix3Template<T> sub3(m_[1], m_[2], m_[3], m_[9], m_[10], m_[11], m_[5], m_[6], m_[7]);

            Matrix3Template<T> sub4(m_[4], m_[6], m_[7], m_[12], m_[14], m_[15], m_[8], m_[10], m_[11]);
            Matrix3Template<T> sub5(m_[0], m_[2], m_[3], m_[8], m_[10], m_[11], m_[12], m_[14], m_[15]);
            Matrix3Template<T> sub6(m_[0], m_[2], m_[3], m_[12], m_[14], m_[15], m_[4], m_[6], m_[7]);
            Matrix3Template<T> sub7(m_[0], m_[2], m_[3], m_[4], m_[6], m_[7], m_[8], m_[10], m_[11]);

            Matrix3Template<T> sub8(m_[4], m_[5], m_[7], m_[8], m_[9], m_[11], m_[12], m_[13], m_[15]);
            Matrix3Template<T> sub9(m_[0], m_[1], m_[3], m_[12], m_[13], m_[15], m_[8], m_[9], m_[11]);
            Matrix3Template<T> sub10(m_[0], m_[1], m_[3], m_[4], m_[5], m_[7], m_[12], m_[13], m_[15]);
            Matrix3Template<T> sub11(m_[0], m_[1], m_[3], m_[8], m_[9], m_[11], m_[4], m_[5], m_[7]);

            Matrix3Template<T> sub12(m_[4], m_[5], m_[6], m_[12], m_[13], m_[14], m_[8], m_[9], m_[10]);
            Matrix3Template<T> sub13(m_[0], m_[1], m_[2], m_[8], m_[9], m_[10], m_[12], m_[13], m_[14]);
            Matrix3Template<T> sub14(m_[0], m_[1], m_[2], m_[12], m_[13], m_[14], m_[4], m_[5], m_[6]);
            Matrix3Template<T> sub15(m_[0], m_[1], m_[2], m_[4], m_[5], m_[6], m_[8], m_[9], m_[10]);

            m_[0] = sub0.determinant() / d;
            m_[1] = sub1.determinant() / d;
            m_[2] = sub2.determinant() / d;
            m_[3] = sub3.determinant() / d;
            m_[4] = sub4.determinant() / d;
            m_[5] = sub5.determinant() / d;
            m_[6] = sub6.determinant() / d;
            m_[7] = sub7.determinant() / d;
            m_[8] = sub8.determinant() / d;
            m_[9] = sub9.determinant() / d;
            m_[10] = sub10.determinant() / d;
            m_[11] = sub11.determinant() / d;
            m_[12] = sub12.determinant() / d;
            m_[13] = sub13.determinant() / d;
            m_[14] = sub14.determinant() / d;
            m_[15] = sub15.determinant() / d;
            return *this;
        }

        operator const T*() const
        {
            return &m_[0];
        }

        bool operator==(const Matrix4Template& r) const
        {
            return m_[0] == r.m_[0] &&
                   m_[1] == r.m_[1] &&
                   m_[2] == r.m_[2] &&
                   m_[3] == r.m_[3] &&
                   m_[4] == r.m_[4] &&
                   m_[5] == r.m_[5] &&
                   m_[6] == r.m_[6] &&
                   m_[7] == r.m_[7] &&
                   m_[8] == r.m_[8] &&
                   m_[9] == r.m_[9] &&
                   m_[10] == r.m_[10] &&
                   m_[11] == r.m_[11] &&
                   m_[12] == r.m_[12] &&
                   m_[13] == r.m_[13] &&
                   m_[14] == r.m_[14] &&
                   m_[15] == r.m_[15];
        }

        bool operator!=(const Matrix4Template& r) const
        {
            return !(*this == r);
        }

        Matrix4Template& operator+=(const Matrix4Template& r)
        {
            Matrix4Template &m = *this;

            m = m + r;
            return m;
        }

        const Matrix4Template operator+(const Matrix4Template& r) const
        {
            T m[] =
            {
                m_[0] + r.m_[0],
                m_[1] + r.m_[1],
                m_[2] + r.m_[2],
                m_[3] + r.m_[3],
                m_[4] + r.m_[4],
                m_[5] + r.m_[5],
                m_[6] + r.m_[6],
                m_[7] + r.m_[7],
                m_[8] + r.m_[8],
                m_[9] + r.m_[9],
                m_[10] + r.m_[10],
                m_[11] + r.m_[11],
                m_[12] + r.m_[12],
                m_[13] + r.m_[13],
                m_[14] + r.m_[14],
                m_[15] + r.m_[15],
            };

            return m;
        }

        Matrix4Template& operator-=(const Matrix4Template& r)
        {
            Matrix4Template &m = *this;
            return m = m - r;
        }

        const Matrix4Template operator-(const Matrix4Template& r) const
        {
            T m[] =
            {
                m_[0] - r.m_[0],
                m_[1] - r.m_[1],
                m_[2] - r.m_[2],
                m_[3] - r.m_[3],
                m_[4] - r.m_[4],
                m_[5] - r.m_[5],
                m_[6] - r.m_[6],
                m_[7] - r.m_[7],
                m_[8] - r.m_[8],
                m_[9] - r.m_[9],
                m_[10] - r.m_[10],
                m_[11] - r.m_[11],
                m_[12] - r.m_[12],
                m_[13] - r.m_[13],
                m_[14] - r.m_[14],
                m_[15] - r.m_[15],
            };

            return m;
        }

        Matrix4Template& operator*=(const Matrix4Template& r)
        {
            Matrix4Template &m = *this;
            return m = m * r;
        }

        const Matrix4Template operator*(const Matrix4Template& r) const
        {
            const T m[] =
            {
                (m_[0] * r.m_[0]) + (m_[4] * r.m_[1]) + (m_[8] * r.m_[2]) + (m_[12] * r.m_[3]),
                (m_[1] * r.m_[0]) + (m_[5] * r.m_[1]) + (m_[9] * r.m_[2]) + (m_[13] * r.m_[3]),
                (m_[2] * r.m_[0]) + (m_[6] * r.m_[1]) + (m_[10] * r.m_[2]) + (m_[14] * r.m_[3]),
                (m_[3] * r.m_[0]) + (m_[7] * r.m_[1]) + (m_[11] * r.m_[2]) + (m_[15] * r.m_[3]),
                (m_[0] * r.m_[4]) + (m_[4] * r.m_[5]) + (m_[8] * r.m_[6]) + (m_[12] * r.m_[7]),
                (m_[1] * r.m_[4]) + (m_[5] * r.m_[5]) + (m_[9] * r.m_[6]) + (m_[13] * r.m_[7]),
                (m_[2] * r.m_[4]) + (m_[6] * r.m_[5]) + (m_[10] * r.m_[6]) + (m_[14] * r.m_[7]),
                (m_[3] * r.m_[4]) + (m_[7] * r.m_[5]) + (m_[11] * r.m_[6]) + (m_[15] * r.m_[7]),
                (m_[0] * r.m_[8]) + (m_[4] * r.m_[9]) + (m_[8] * r.m_[10]) + (m_[12] * r.m_[11]),
                (m_[1] * r.m_[8]) + (m_[5] * r.m_[9]) + (m_[9] * r.m_[10]) + (m_[13] * r.m_[11]),
                (m_[2] * r.m_[8]) + (m_[6] * r.m_[9]) + (m_[10] * r.m_[10]) + (m_[14] * r.m_[11]),
                (m_[3] * r.m_[8]) + (m_[7] * r.m_[9]) + (m_[11] * r.m_[10]) + (m_[15] * r.m_[11]),
                (m_[0] * r.m_[12]) + (m_[4] * r.m_[13]) + (m_[8] * r.m_[14]) + (m_[12] * r.m_[15]),
                (m_[1] * r.m_[12]) + (m_[5] * r.m_[13]) + (m_[9] * r.m_[14]) + (m_[13] * r.m_[15]),
                (m_[2] * r.m_[12]) + (m_[6] * r.m_[13]) + (m_[10] * r.m_[14]) + (m_[14] * r.m_[15]),
                (m_[3] * r.m_[12]) + (m_[7] * r.m_[13]) + (m_[11] * r.m_[14]) + (m_[15] * r.m_[15]),
            };

            return m;
        }

        Matrix4Template& operator*=(const T& r)
        {
            Matrix4Template &m = *this;
            return m = m * r;
        }

        const Matrix4Template operator*(const T& r) const
        {
            const T m[] =
            {
                m_[0] * r,
                m_[1] * r,
                m_[2] * r,
                m_[3] * r,
                m_[4] * r,
                m_[5] * r,
                m_[6] * r,
                m_[7] * r,
                m_[8] * r,
                m_[9] * r,
                m_[10] * r,
                m_[11] * r,
                m_[12] * r,
                m_[13] * r,
                m_[14] * r,
                m_[15] * r,
            };

            return m;
        }

        Matrix4Template& operator/=(const T& r)
        {
            Matrix4Template &m = *this;
            return m = m / r;
        }

        const Matrix4Template operator/(const T& r) const
        {
            const T m[] =
            {
                m_[0] / r,
                m_[1] / r,
                m_[2] / r,
                m_[3] / r,
                m_[4] / r,
                m_[5] / r,
                m_[6] / r,
                m_[7] / r,
                m_[8] / r,
                m_[9] / r,
                m_[10] / r,
                m_[11] / r,
                m_[12] / r,
                m_[13] / r,
                m_[14] / r,
                m_[15] / r,
            };

            return m;
        }

        ProxyMatrix<T, 4> operator[](int index)
        {
            return ProxyMatrix<T, 4>(&m_[index]);
        }

        const ProxyMatrix<T, 4> operator[](int index) const
        {
            return ProxyMatrix<T, 4>(const_cast<T*>(&m_[index]));
        }

        void print() const
        {
            Log::I("| %8.3f %8.3f %8.3f %8.3f |\n| %8.3f %8.3f %8.3f %8.3f |\n"
                   "| %8.3f %8.3f %8.3f %8.3f |\n| %8.3f %8.3f %8.3f %8.3f |\n",
                   m_[0], m_[4], m_[8], m_[12], m_[1], m_[5], m_[9], m_[13],
                   m_[2], m_[6], m_[10], m_[14], m_[3], m_[7], m_[11], m_[15]);
        }

        QuaternionTemplate<T> quaternion() const
        {
            T trace = 1 + m_[0] + m_[5] + m_[10];
            T x, y, z, w;

            if (trace > 0)
            {
                  T s = sqrt(trace) * 2;
                  x = (m_[9] - m_[6]) / s;
                  y = (m_[2] - m_[8]) / s;
                  z = (m_[4] - m_[1]) / s;
                  w = 0.25f * s;
            }
            else
            {
                if (m_[0] > m_[5] && m_[0] > m_[10])
                {
                    // Column 0:
                    T s = sqrt(1.0 + m_[0] - m_[5] - m_[10]) * 2;
                    x = 0.25 * s;
                    y = (m_[4] + m_[1]) / s;
                    z = (m_[2] + m_[8]) / s;
                    w = (m_[9] - m_[6]) / s;
                }
                else if (m_[5] > m_[10])
                {
                    // Column 1:
                    T s = sqrt(1.0 + m_[5] - m_[0] - m_[10]) * 2;
                    x = (m_[4] + m_[1]) / s;
                    y = 0.25 * s;
                    z = (m_[9] + m_[6]) / s;
                    w = (m_[2] - m_[8]) / s;
                }
                else
                {
                    // Column 2:
                    T s = sqrt(1.0 + m_[10] - m_[0] - m_[5]) * 2;
                    x = (m_[2] + m_[8]) / s;
                    y = (m_[9] + m_[6]) / s;
                    z = 0.25 * s;
                    w = (m_[4] - m_[1]) / s;
                }
            }

            return QuaternionTemplate<T>(x, y, z, w);
        }

    private:
        T m_[16];
    };

    template<typename T> const Matrix4Template<T> operator*(const T& l,
        const Matrix4Template<T>& r)
    {
        return Matrix4Template<T>(r) * l;
    }

    template<typename T> const Vector4Template<T> operator*(const Vector4Template<T>& l,
        const Matrix4Template<T>& r)
    {
        T x((l.x() * r[0][0]) + (l.y() * r[1][0]) + (l.z() * r[2][0]) + (l.w() * r[3][0]));
        T y((l.x() * r[0][1]) + (l.y() * r[1][1]) + (l.z() * r[2][1]) + (l.w() * r[3][1]));
        T z((l.x() * r[0][2]) + (l.y() * r[1][2]) + (l.z() * r[2][2]) + (l.w() * r[3][2]));
        T w((l.x() * r[0][3]) + (l.y() * r[1][3]) + (l.z() * r[2][3]) + (l.w() * r[3][3]));
        return Vector4Template<T>(x, y, z, w);
    }

    template<typename T> const Vector4Template<T> operator*(const Matrix4Template<T>& l,
        const Vector4Template<T>& r)
    {
        T x((l[0][0] * r.x()) + (l[0][1] * r.y()) + (l[0][2] * r.z()) + (l[0][3] * r.w()));
        T y((l[1][0] * r.x()) + (l[1][1] * r.y()) + (l[1][2] * r.z()) + (l[1][3] * r.w()));
        T z((l[2][0] * r.x()) + (l[2][1] * r.y()) + (l[2][2] * r.z()) + (l[2][3] * r.w()));
        T w((l[3][0] * r.x()) + (l[3][1] * r.y()) + (l[3][2] * r.z()) + (l[3][3] * r.w()));
        return Vector4Template<T>(x, y, z, w);
    }

    template<typename T> const Matrix4Template<T> outer(const Vector4Template<T>& a,
        const Vector4Template<T>& b)
    {
        Matrix4Template<T> r;
        r[0][0] = a.x() * b.x();
        r[0][1] = a.x() * b.y();
        r[0][2] = a.x() * b.z();
        r[0][3] = a.x() * b.w();
        r[1][0] = a.y() * b.x();
        r[1][1] = a.y() * b.y();
        r[1][2] = a.y() * b.z();
        r[1][3] = a.y() * b.w();
        r[2][0] = a.z() * b.x();
        r[2][1] = a.z() * b.y();
        r[2][2] = a.z() * b.z();
        r[2][3] = a.z() * b.w();
        r[3][0] = a.w() * b.x();
        r[3][1] = a.w() * b.y();
        r[3][2] = a.w() * b.z();
        r[3][3] = a.w() * b.w();
        return r;
    }

    template<typename T> const Matrix4Template<T> slerp(Matrix4Template<T> const& a, Matrix4Template<T> const& b, float t)
    {
        QuaternionTemplate<T> qa = a.quaternion();
        QuaternionTemplate<T> qb = b.quaternion();
        QuaternionTemplate<T> tmp = Quaternion::slerp(qa, qb, t);
        return toMatrix(tmp);
    }

    template<typename T> const Matrix4Template<T> toMatrix(QuaternionTemplate<T> const& q)
    {
        T qx = q.x();
        T qy = q.y();
        T qz = q.z();
        T qw = q.w();

        const T n = 1.0f / sqrt(qx*qx + qy*qy + qz*qz + qw*qw);
        qx *= n;
        qy *= n;
        qz *= n;
        qw *= n;

        T m[16];
        m[0]  =   1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
        m[1]  =   2.0f*qx*qy - 2.0f*qz*qw;
        m[2]  =   2.0f*qx*qz + 2.0f*qy*qw;
        m[3]  =   0.0f;

        m[4]  =   2.0f*qx*qy + 2.0f*qz*qw;
        m[5]  =   1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
        m[6]  =   2.0f*qy*qz - 2.0f*qx*qw;
        m[7]  =   0.0f;

        m[8]  =   2.0f*qx*qz - 2.0f*qy*qw;
        m[9]  =   2.0f*qy*qz + 2.0f*qx*qw;
        m[10] =    1.0f - 2.0f*qx*qx - 2.0f*qy*qy;
        m[11] =    0.0f;

        m[12] =    0.0f;
        m[13] =    0.0f;
        m[14] =    0.0f;
        m[15] =    1.0f;

        return Matrix4Template<T>(m);
    }
}

#endif
