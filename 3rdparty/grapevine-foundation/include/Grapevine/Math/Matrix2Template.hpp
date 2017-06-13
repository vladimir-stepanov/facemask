/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_MATRIX2TEMPLATE
#define GRAPEVINE_MATH_MATRIX2TEMPLATE

#include "Grapevine/Math/ProxyMatrix.hpp"
#include "Grapevine/Math/Vector2Template.hpp"
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace Grapevine
{
    template<typename T> class Matrix2Template
    {
    public:
        // CLASS_COPYABLE

        Matrix2Template()
        {
            setIdentity();
        }

        Matrix2Template(const T& c0r0, const T& c0r1, const T& c1r0, const T& c1r1)
        {
            m_[0] = c0r0;
            m_[1] = c0r1;
            m_[2] = c1r0;
            m_[3] = c1r1;
        }

        Matrix2Template(const T m[])
        {
            m_[0] = m[0];
            m_[1] = m[1];
            m_[2] = m[2];
            m_[3] = m[3];
        }

        void setIdentity()
        {
            m_[0] = 1;
            m_[1] = 0;
            m_[2] = 0;
            m_[3] = 1;
        }

        Matrix2Template& transpose()
        {
            T tmp = m_[1];
            m_[1] = m_[2];
            m_[2] = tmp;
            return *this;
        }

        T determinant()
        {
            return (m_[0] * m_[3]) - (m_[2] * m_[1]);
        }

        Matrix2Template& inverse()
        {
            T d(determinant());
            if (d == static_cast<T>(0))
            {
                throw std::runtime_error("TMatrix is noninvertible.");
            }
            T c0r0(m_[3] / d);
            T c0r1(-m_[1] / d);
            T c1r0(-m_[2] / d);
            T c1r1(m_[0] / d);
            m_[0] = c0r0;
            m_[1] = c0r1;
            m_[2] = c1r0;
            m_[3] = c1r1;
            return *this;
        }

        operator const T*() const { return &m_[0];}

        bool operator==(const Matrix2Template& r) const
        {
            return m_[0] == r.m_[0] &&
                   m_[1] == r.m_[1] &&
                   m_[2] == r.m_[2] &&
                   m_[3] == r.m_[3];
        }

        bool operator!=(const Matrix2Template& r) const
        {
            return !(*this == r);
        }

        Matrix2Template& operator+=(const Matrix2Template& r)
        {
            Matrix2Template &m = *this;

            m = m + r;
            return m;
        }

        const Matrix2Template operator+(const Matrix2Template& r) const
        {
            T m[] =
            {
                m_[0] + r.m_[0],
                m_[1] + r.m_[1],
                m_[2] + r.m_[2],
                m_[3] + r.m_[3],
            };

            return m;
        }

        Matrix2Template& operator-=(const Matrix2Template& r)
        {
            Matrix2Template &m = *this;
            return m = m - r;
        }

        const Matrix2Template operator-(const Matrix2Template& r) const
        {
            T m[] =
            {
                m_[0] - r.m_[0],
                m_[1] - r.m_[1],
                m_[2] - r.m_[2],
                m_[3] - r.m_[3],
            };

            return m;
        }

        Matrix2Template& operator*=(const Matrix2Template& r)
        {
            Matrix2Template &m = *this;
            return m = m * r;
        }

        const Matrix2Template operator*(const Matrix2Template& r) const
        {
            const T m[] =
            {
                (m_[0] * r.m_[0]) + (m_[2] * r.m_[1]),
                (m_[1] * r.m_[0]) + (m_[3] * r.m_[1]),
                (m_[0] * r.m_[2]) + (m_[2] * r.m_[3]),
                (m_[1] * r.m_[2]) + (m_[3] * r.m_[3])
            };

            return m;
        }

        Matrix2Template& operator*=(const T& r)
        {
            Matrix2Template &m = *this;
            return m = m * r;
        }

        const Matrix2Template operator*(const T& r) const
        {
            const T m[] =
            {
                m_[0] * r,
                m_[1] * r,
                m_[2] * r,
                m_[3] * r,
            };

            return m;
        }

        Matrix2Template& operator/=(const T& r)
        {
            Matrix2Template &m = *this;
            return m = m / r;
        }

        const Matrix2Template operator/(const T& r) const
        {
            const T m[] =
            {
                m_[0] / r,
                m_[1] / r,
                m_[2] / r,
                m_[3] / r,
            };

            return m;
        }

        ProxyMatrix<T, 2> operator[](int index)
        {
            return ProxyMatrix<T, 2>(&m_[index]);
        }

        const ProxyMatrix<T, 2> operator[](int index) const
        {
            return ProxyMatrix<T, 2>(const_cast<T*>(&m_[index]));
        }

    private:
        T m_[4];
    };

    template<typename T> const Matrix2Template<T> operator*(const T& l,
        const Matrix2Template<T>& r)
    {
        return Matrix2Template<T>(r) * l;
    }

    template<typename T> const Vector2Template<T> operator*(const Vector2Template<T>& l,
        const Matrix2Template<T>& r)
    {
        T x((l.x() * r[0][0]) + (l.y() * r[1][0]));
        T y((l.x() * r[0][1]) + (l.y() * r[1][1]));
        return Vector2Template<T>(x,y);
    }

    template<typename T> const Vector2Template<T> operator*(const Matrix2Template<T>& l,
        const Vector2Template<T>& r)
    {
        T x((l[0][0] * r.x()) + (l[0][1] * r.y()));
        T y((l[1][0] * r.x()) + (l[1][1] * r.y()));
        return Vector2Template<T>(x, y);
    }

    template<typename T> const Matrix2Template<T> outer(const Vector2Template<T>& a,
        const Vector2Template<T>& b)
    {
        Matrix2Template<T> r;
        r[0][0] = a.x() * b.x();
        r[0][1] = a.x() * b.y();
        r[1][0] = a.y() * b.x();
        r[1][1] = a.y() * b.y();
        return r;
    }
}

#endif
