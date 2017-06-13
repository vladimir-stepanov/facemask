/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_MATRIX3TEMPLATE
#define GRAPEVINE_MATH_MATRIX3TEMPLATE

#include "Grapevine/Math/Matrix2Template.hpp"
#include "Grapevine/Math/Vector3Template.hpp"

#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace Grapevine
{
    template<typename T> class Matrix3Template
    {
    public:
        // CLASS_COPYABLE

        Matrix3Template()
        {
            setIdentity();
        }

        Matrix3Template(const T& c0r0, const T& c0r1, const T& c0r2,
            const T& c1r0, const T& c1r1, const T& c1r2,
            const T& c2r0, const T& c2r1, const T& c2r2)
        {
            m_[0] = c0r0;
            m_[1] = c0r1;
            m_[2] = c0r2;
            m_[3] = c1r0;
            m_[4] = c1r1;
            m_[5] = c1r2;
            m_[6] = c2r0;
            m_[7] = c2r1;
            m_[8] = c2r2;
        }

        Matrix3Template(const T m[])
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
        }

        void set(unsigned int r, unsigned int c, T value)
        {
            m_[c * 3 + r] = value;
        }

        void set(unsigned int index, T value)
        {
            m_[index] = value;
        }

        T get(unsigned int index) const
        {
            return m_[index];
        }

        void setIdentity()
        {
            m_[0] = 1;
            m_[1] = 0;
            m_[2] = 0;
            m_[3] = 0;
            m_[4] = 1;
            m_[5] = 0;
            m_[6] = 0;
            m_[7] = 0;
            m_[8] = 1;
        }

        Matrix3Template& transpose()
        {
            T tmp = m_[1];
            m_[1] = m_[3];
            m_[3] = tmp;

            tmp = m_[2];
            m_[2] = m_[6];
            m_[6] = tmp;

            tmp = m_[5];
            m_[5] = m_[7];
            m_[7] = tmp;

            return *this;
        }

        T determinant()
        {
            Matrix2Template<T> sub0(m_[4], m_[5], m_[7], m_[8]);
            Matrix2Template<T> sub3(m_[1], m_[2], m_[7], m_[8]);
            Matrix2Template<T> sub6(m_[1], m_[2], m_[4], m_[5]);
            return (m_[0] * sub0.determinant()) - (m_[3] * sub3.determinant()) + (m_[6] * sub6.determinant());
        }

        Matrix3Template& inverse() throw(std::runtime_error)
        {
            T d(determinant());
            if (d == static_cast<T>(0))
            {
                throw std::runtime_error("TMatrix is noninvertible!");
            }
            Matrix2Template<T> sub0(m_[4], m_[5], m_[7], m_[8]);
            Matrix2Template<T> sub1(m_[7], m_[8], m_[1], m_[2]);
            Matrix2Template<T> sub2(m_[1], m_[2], m_[4], m_[5]);
            Matrix2Template<T> sub3(m_[6], m_[8], m_[3], m_[5]);
            Matrix2Template<T> sub4(m_[0], m_[2], m_[6], m_[8]);
            Matrix2Template<T> sub5(m_[3], m_[5], m_[0], m_[2]);
            Matrix2Template<T> sub6(m_[3], m_[4], m_[6], m_[7]);
            Matrix2Template<T> sub7(m_[6], m_[7], m_[0], m_[1]);
            Matrix2Template<T> sub8(m_[0], m_[1], m_[3], m_[4]);
            m_[0] = sub0.determinant() / d;
            m_[1] = sub1.determinant() / d;
            m_[2] = sub2.determinant() / d;
            m_[3] = sub3.determinant() / d;
            m_[4] = sub4.determinant() / d;
            m_[5] = sub5.determinant() / d;
            m_[6] = sub6.determinant() / d;
            m_[7] = sub7.determinant() / d;
            m_[8] = sub8.determinant() / d;
            return *this;
        }

        operator const T*() const { return &m_[0];}

        bool operator==(const Matrix3Template& r) const
        {
            return m_[0] == r.m_[0] &&
                   m_[1] == r.m_[1] &&
                   m_[2] == r.m_[2] &&
                   m_[3] == r.m_[3] &&
                   m_[4] == r.m_[4] &&
                   m_[5] == r.m_[5] &&
                   m_[6] == r.m_[6] &&
                   m_[7] == r.m_[7] &&
                   m_[8] == r.m_[8];
        }

        bool operator!=(const Matrix3Template& r) const
        {
            return !(*this == r);
        }

        Matrix3Template& operator+=(const Matrix3Template& r)
        {
            Matrix3Template &m = *this;

            m = m + r;
            return m;
        }

        const Matrix3Template operator+(const Matrix3Template& r) const
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
            };

            return m;
        }

        Matrix3Template& operator-=(const Matrix3Template& r)
        {
            Matrix3Template &m = *this;
            return m = m - r;
        }

        const Matrix3Template operator-(const Matrix3Template& r) const
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
            };

            return m;
        }

        Matrix3Template& operator*=(const Matrix3Template& r)
        {
            Matrix3Template &m = *this;
            return m = m * r;
        }

        const Matrix3Template operator*(const Matrix3Template& r) const
        {
            const T m[] =
            {
                (m_[0] * r.m_[0]) + (m_[3] * r.m_[1]) + (m_[6] * r.m_[2]),
                (m_[1] * r.m_[0]) + (m_[4] * r.m_[1]) + (m_[7] * r.m_[2]),
                (m_[2] * r.m_[0]) + (m_[5] * r.m_[1]) + (m_[8] * r.m_[2]),
                (m_[0] * r.m_[3]) + (m_[3] * r.m_[4]) + (m_[6] * r.m_[5]),
                (m_[1] * r.m_[3]) + (m_[4] * r.m_[4]) + (m_[7] * r.m_[5]),
                (m_[2] * r.m_[3]) + (m_[5] * r.m_[4]) + (m_[8] * r.m_[5]),
                (m_[0] * r.m_[6]) + (m_[3] * r.m_[7]) + (m_[6] * r.m_[8]),
                (m_[1] * r.m_[6]) + (m_[4] * r.m_[7]) + (m_[7] * r.m_[8]),
                (m_[2] * r.m_[6]) + (m_[5] * r.m_[7]) + (m_[8] * r.m_[8]),
            };

            return m;
        }

        Matrix3Template& operator*=(const T& r)
        {
            Matrix3Template &m = *this;
            return m = m * r;
        }

        const Matrix3Template operator*(const T& r) const
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
            };

            return m;
        }

        Matrix3Template& operator/=(const T& r)
        {
            Matrix3Template &m = *this;
            return m = m / r;
        }

        const Matrix3Template operator/(const T& r) const
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
            };

            return m;
        }

        ProxyMatrix<T, 3> operator[](int index)
        {
            return ProxyMatrix<T, 3>(&m_[index]);
        }

        const ProxyMatrix<T, 3> operator[](int index) const
        {
            return ProxyMatrix<T, 3>(const_cast<T*>(&m_[index]));
        }

    private:
        T m_[9];
    };

    template<typename T> const Matrix3Template<T> operator*(const T& l,
        const Matrix3Template<T>& r)
    {
        return Matrix3Template<T>(r) * l;
    }

    template<typename T> const Vector3Template<T> operator*(const Vector3Template<T>& l,
        const Matrix3Template<T>& r)
    {
        T x((l.x() * r[0][0]) + (l.y() * r[1][0]) + (l.z() * r[2][0]));
        T y((l.x() * r[0][1]) + (l.y() * r[1][1]) + (l.z() * r[2][1]));
        T z((l.x() * r[0][2]) + (l.y() * r[1][2]) + (l.z() * r[2][2]));
        return Vector3Template<T>(x, y, z);
    }

    template<typename T> const Vector3Template<T> operator*(const Matrix3Template<T>& l,
        const Vector3Template<T>& r)
    {
        T x((l[0][0] * r.x()) + (l[0][1] * r.y()) + (l[0][2] * r.z()));
        T y((l[1][0] * r.x()) + (l[1][1] * r.y()) + (l[1][2] * r.z()));
        T z((l[2][0] * r.x()) + (l[2][1] * r.y()) + (l[2][2] * r.z()));
        return Vector3Template<T>(x, y, z);
    }

    template<typename T> const Matrix3Template<T> outer(const Vector3Template<T>& a,
        const Vector3Template<T>& b)
    {
        Matrix3Template<T> r;
        r[0][0] = a.x() * b.x();
        r[0][1] = a.x() * b.y();
        r[0][2] = a.x() * b.z();
        r[1][0] = a.y() * b.x();
        r[1][1] = a.y() * b.y();
        r[1][2] = a.y() * b.z();
        r[2][0] = a.z() * b.x();
        r[2][1] = a.z() * b.y();
        r[2][2] = a.z() * b.z();
        return r;
    }
}

#endif
