/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_VECTOR4TEMPLATE
#define GRAPEVINE_MATH_VECTOR4TEMPLATE

#include "Grapevine/Math/Vector3Template.hpp"
#include <stdexcept>

namespace Grapevine
{
    template<typename T> class Vector4Template
    {
    public:
        // CLASS_COPYABLE

        Vector4Template():
            x_(0),
            y_(0),
            z_(0),
            w_(0)
        {
        }

        Vector4Template(T t):
            x_(t),
            y_(t),
            z_(t),
            w_(t)
        {
        }

        Vector4Template(T x, T y, T z, T w):
            x_(x),
            y_(y),
            z_(z),
            w_(w)
        {
        }

        operator const T*() const
        {
            return &x_;
        }

        const T x() const { return x_; }
        const T y() const { return y_; }
        const T z() const { return z_; }
        const T w() const { return w_; }

        const T r() const { return x_; }
        const T g() const { return y_; }
        const T b() const { return z_; }
        const T a() const { return w_; }

        void x(const T& val) { x_ = val; }
        void y(const T& val) { y_ = val; }
        void z(const T& val) { z_ = val; }
        void w(const T& val) { w_ = val; }

        void r(const T& val) { x_ = val; }
        void g(const T& val) { y_ = val; }
        void b(const T& val) { z_ = val; }
        void a(const T& val) { w_ = val; }

        Vector4Template& operator/=(const T& r)
        {
            x_ /= r;
            y_ /= r;
            z_ /= r;
            w_ /= r;
            return *this;
        }

        const Vector4Template operator/(const T& r) const
        {
            return Vector4Template(*this) /= r;
        }

        Vector4Template& operator/=(Vector4Template const& r)
        {
            x_ /= r.x_;
            y_ /= r.y_;
            z_ /= r.z_;
            w_ /= r.w_;
            return *this;
        }

        const Vector4Template operator/(Vector4Template const& r) const
        {
            return Vector4Template(*this) /= r;
        }

        Vector4Template& operator*=(const T& r)
        {
            x_ *= r;
            y_ *= r;
            z_ *= r;
            w_ *= r;
            return *this;
        }

        const Vector4Template operator*(const T& r) const
        {
            return Vector4Template(*this) *= r;
        }

        Vector4Template& operator*=(Vector4Template const& r)
        {
            x_ *= r.x_;
            y_ *= r.y_;
            z_ *= r.z_;
            w_ *= r.w_;
            return *this;
        }

        const Vector4Template operator*(Vector4Template const& r) const
        {
            return Vector4Template(*this) *= r;
        }

        Vector4Template& operator+=(const T& r)
        {
            x_ += r;
            y_ += r;
            z_ += r;
            w_ += r;
            return *this;
        }

        const Vector4Template operator+(const T& r) const
        {
            return Vector4Template(*this) += r;
        }

        Vector4Template& operator+=(Vector4Template const& r)
        {
            x_ += r.x_;
            y_ += r.y_;
            z_ += r.z_;
            w_ += r.w_;
            return *this;
        }

        const Vector4Template operator+(Vector4Template const& r) const
        {
            return Vector4Template(*this) += r;
        }

        Vector4Template& operator-=(const T& r)
        {
            x_ -= r;
            y_ -= r;
            z_ -= r;
            w_ -= r;
            return *this;
        }

        const Vector4Template operator-(const T& r) const
        {
            return Vector4Template(*this) -= r;
        }

        Vector4Template& operator-=(Vector4Template const& r)
        {
            x_ -= r.x_;
            y_ -= r.y_;
            z_ -= r.z_;
            w_ -= r.w_;
            return *this;
        }

        const Vector4Template operator-(Vector4Template const& r) const
        {
            return Vector4Template(*this) -= r;
        }

        float length() const
        {
            return sqrt(dot(*this, *this));
        }

        Vector4Template normal() const
        {
            float l = length();
            return Vector4Template(x_ / l, y_ / l, z_ / l, w_ / l);
        }

        void normalize()
        {
            float l = length();
            x_ /= l;
            y_ /= l;
            z_ /= l;
            w_ /= l;
        }

        static T dot(Vector4Template const& v1, Vector4Template const& v2)
        {
            return (v1.x_ * v2.x_) + (v1.y_ * v2.y_) + (v1.z_ * v2.z_) + (v1.w_ * v2.w_);
        }

        T &operator[](int index) throw(std::runtime_error)
        {
            switch(index)
            {
                case 0: return x_;
                case 1: return y_;
                case 2: return z_;
                case 3: return w_;
                default:
                    throw std::runtime_error("Invalid vector index");
            }
        }

        bool operator==(Vector4Template const& r) const
        {
            return r.x() == x() && r.y() == y() && r.z() == z() && r.w() == w();
        }

        bool operator!=(Vector4Template const& r) const
        {
            return !(*this == r);
        }

    private:
        T x_;
        T y_;
        T z_;
        T w_;
    };

    template<typename T> Vector4Template<T> operator*(float multiplier, const Vector4Template<T>& r)
    {
        return r * multiplier;
    }
}

#endif
