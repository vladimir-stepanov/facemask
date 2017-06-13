/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_VECTOR2TEMPLATE
#define GRAPEVINE_MATH_VECTOR2TEMPLATE

#include <cmath>

namespace Grapevine
{
    template<typename T> class Vector2Template
    {
    public:
        // CLASS_COPYABLE

        Vector2Template(): x_(0), y_(0)
        {
        }

        Vector2Template(T t): x_(t), y_(t)
        {
        }

        Vector2Template(T x, T y): x_(x), y_(y)
        {
        }

        operator T*()
        {
            return &x_;
        }

        operator const T*() const
        {
            return &x_;
        }

        const T x() const
        {
            return x_;
        }

        const T y() const
        {
            return y_;
        }

        void x(const T& val)
        {
            x_ = val;
        }

        void y(const T& val)
        {
            y_ = val;
        }

        float length() const
        {
            return sqrt(dot(*this, *this));
        }

        Vector2Template normal() const
        {
            float l = length();
            return Vector2Template(x_ / l, y_ / l);
        }

        void normalize()
        {
            float l = length();
            x_ /= l;
            y_ /= l;
        }

        static T dot(Vector2Template const& v1, Vector2Template const& v2)
        {
            return (v1.x_ * v2.x_) + (v1.y_ * v2.y_);
        }

        Vector2Template& operator/=(const T& r)
        {
            x_ /= r;
            y_ /= r;
            return *this;
        }

        const Vector2Template operator/(const T& r) const
        {
            return Vector2Template(*this) /= r;
        }

        Vector2Template& operator/=(Vector2Template const& r)
        {
            x_ /= r.x_;
            y_ /= r.y_;
            return *this;
        }

        const Vector2Template operator/(Vector2Template const& r) const
        {
            return Vector2Template(*this) /= r;
        }

        Vector2Template& operator*=(const T& r)
        {
            x_ *= r;
            y_ *= r;
            return *this;
        }

        const Vector2Template operator*(const T& r) const
        {
            return Vector2Template(*this) *= r;
        }

        Vector2Template& operator*=(Vector2Template const& r)
        {
            x_ *= r.x_;
            y_ *= r.y_;
            return *this;
        }

        const Vector2Template operator*(Vector2Template const& r) const
        {
            return Vector2Template(*this) *= r;
        }

        Vector2Template& operator+=(const T& r)
        {
            x_ += r;
            y_ += r;
            return *this;
        }

        const Vector2Template operator+(const T& r) const
        {
            return Vector2Template(*this) += r;
        }

        Vector2Template& operator+=(Vector2Template const& r)
        {
            x_ += r.x_;
            y_ += r.y_;
            return *this;
        }

        const Vector2Template operator+(Vector2Template const& r) const
        {
            return Vector2Template(*this) += r;
        }

        Vector2Template& operator-=(const T& r)
        {
            x_ -= r;
            y_ -= r;
            return *this;
        }

        const Vector2Template operator-(const T& r) const
        {
            return Vector2Template(*this) -= r;
        }

        Vector2Template& operator-=(Vector2Template const& r)
        {
            x_ -= r.x_;
            y_ -= r.y_;
            return *this;
        }

        const Vector2Template operator-(Vector2Template const& r) const
        {
            return Vector2Template(*this) -= r;
        }

        bool operator==(Vector2Template const& r) const
        {
            return r.x() == x() && r.y() == y();
        }

        bool operator!=(Vector2Template const& r) const
        {
            return !(*this == r);
        }

    private:
        T x_;
        T y_;
    };

    template<typename T> Vector2Template<T> operator*(float multiplier, const Vector2Template<T>& r)
    {
        return r * multiplier;
    }
}

#endif
