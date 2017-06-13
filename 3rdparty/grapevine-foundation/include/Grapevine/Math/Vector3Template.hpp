/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_VECTOR3TEMPLATE
#define GRAPEVINE_MATH_VECTOR3TEMPLATE

#include "Grapevine/Math/Vector2Template.hpp"
#include <stdexcept>

namespace Grapevine
{
    template<typename T> class Vector3Template
    {
    public:
        // CLASS_COPYABLE

        Vector3Template():
            x_(0),
            y_(0),
            z_(0)
        {
        }

        Vector3Template(T t):
            x_(t),
            y_(t),
            z_(t)
        {
        }

        Vector3Template(T x, T y, T z):
            x_(x),
            y_(y),
            z_(z)
        {
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

        const T z() const
        {
            return z_;
        }

        const T r() const
        {
            return x_;
        }

        const T g() const
        {
            return y_;
        }

        const T b() const
        {
            return z_;
        }

        void x(const T& val)
        {
            x_ = val;
        }

        void y(const T& val)
        {
            y_ = val;
        }

        void z(const T& val)
        {
            z_ = val;
        }

        void r(const T& val)
        {
            x_ = val;
        }

        void g(const T& val)
        {
            y_ = val;
        }

        void b(const T& val)
        {
            z_ = val;
        }

        float length() const
        {
            return sqrt(dot(*this, *this));
        }

        float squaredLength() const
        {
            return x_ * x_ + y_ * y_ + z_ * z_;
        }

        Vector3Template normal() const
        {
            float l = length();
            return Vector3Template(x_ / l, y_ / l, z_ / l);
        }

        void normalize()
        {
            float l = length();
            x_ /= l;
            y_ /= l;
            z_ /= l;
        }

        static T dot(Vector3Template const& v1, Vector3Template const& v2)
        {
            return (v1.x_ * v2.x_) + (v1.y_ * v2.y_) + (v1.z_ * v2.z_);
        }

        static Vector3Template cross(Vector3Template const& u,
            Vector3Template const& v)
        {
            return Vector3Template((u.y_ * v.z_) - (u.z_ * v.y_),
                (u.z_ * v.x_) - (u.x_ * v.z_),
                (u.x_ * v.y_) - (u.y_ * v.x_));
        }

        static T distance(Vector3Template const& v1,Vector3Template const& v2)
        {
            return sqrt(pow(v2.x()-v1.x(),2)+pow(v2.y()-v1.y(),2)+pow(v2.z()-v1.z(),2));
        }

        Vector3Template& operator/=(T const& r)
        {
            x_ /= r;
            y_ /= r;
            z_ /= r;
            return *this;
        }

        const Vector3Template operator/(const T& r) const
        {
            return Vector3Template(*this) /= r;
        }

        Vector3Template& operator/=(Vector3Template const& r)
        {
            x_ /= r.x_;
            y_ /= r.y_;
            z_ /= r.z_;
            return *this;
        }

        const Vector3Template operator/(Vector3Template const& r) const
        {
            return Vector3Template(*this) /= r;
        }

        Vector3Template& operator*=(const T& r)
        {
            x_ *= r;
            y_ *= r;
            z_ *= r;
            return *this;
        }

        const Vector3Template operator*(const T& r) const
        {
            return Vector3Template(*this) *= r;
        }

        Vector3Template& operator*=(Vector3Template const& r)
        {
            x_ *= r.x_;
            y_ *= r.y_;
            z_ *= r.z_;
            return *this;
        }

        const Vector3Template operator*(Vector3Template const& r) const
        {
            return Vector3Template(*this) *= r;
        }

        Vector3Template& operator+=(const T& r)
        {
            x_ += r;
            y_ += r;
            z_ += r;
            return *this;
        }

        const Vector3Template operator+(const T& r) const
        {
            return Vector3Template(*this) += r;
        }

        Vector3Template& operator+=(Vector3Template const& r)
        {
            x_ += r.x_;
            y_ += r.y_;
            z_ += r.z_;
            return *this;
        }

        const Vector3Template operator+(Vector3Template const& r) const
        {
            return Vector3Template(*this) += r;
        }

        Vector3Template& operator-=(const T& r)
        {
            x_ -= r;
            y_ -= r;
            z_ -= r;
            return *this;
        }

        const Vector3Template operator-(const T& r) const
        {
            return Vector3Template(*this) -= r;
        }

        Vector3Template& operator-=(Vector3Template const& r)
        {
            x_ -= r.x_;
            y_ -= r.y_;
            z_ -= r.z_;
            return *this;
        }

        const Vector3Template operator-(Vector3Template const& r) const
        {
            return Vector3Template(*this) -= r;
        }

        T &operator[](int index) throw(std::runtime_error)
        {
            switch(index)
            {
                case 0: return x_;
                case 1: return y_;
                case 2: return z_;
                default:
                throw std::runtime_error("Invalid vector index");
            }
        }

        bool operator==(Vector3Template const& r) const
        {
            return r.x() == x() && r.y() == y() && r.z() == z();
        }

        bool operator!=(Vector3Template const& r) const
        {
            return !(*this == r);
        }
    private:
        T x_;
        T y_;
        T z_;
    };

    template<typename T> Vector3Template<T> operator*(float multiplier, const Vector3Template<T>& r)
    {
        return r * multiplier;
    }
}

#endif
