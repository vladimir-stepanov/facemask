/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_MATH_QUATERNIONTEMPLATE
#define GRAPEVINE_MATH_QUATERNIONTEMPLATE

#include <cmath>

namespace Grapevine
{
    template<typename T> class QuaternionTemplate
    {
    public:
        // CLASS_COPYABLE

        QuaternionTemplate(T x, T y, T z, T w):
            x_(x),
            y_(y),
            z_(z),
            w_(w)
        {
        }

        static QuaternionTemplate<T> slerp(QuaternionTemplate const& qa, QuaternionTemplate const& qb, double t)
        {
            // Calculate angle between qa and qb
            double cosHalfTheta = qa.w_ * qb.w_ + qa.x_ * qb.x_ + qa.y_ * qb.y_ + qa.z_ * qb.z_;

            // If qa == qb or qa == -qb then theta = 0 and we can return qa
            if ((float)std::abs(cosHalfTheta) >= 1.0f)
            {
                return QuaternionTemplate<T>(qa.x_, qa.y_, qa.z_, qa.w_);
            }

            // Calculate temporary values.
            double halfTheta = acos(cosHalfTheta);
            double sinHalfTheta = sqrt(1.0f - cosHalfTheta*cosHalfTheta);

            // If theta = 180 degrees then result is not defined
            // we could rotate around any axis normal to qa or qb
            if ((float)std::abs(sinHalfTheta) < 0.001f)
            {
                return QuaternionTemplate<T>(qa.x_ * 0.5f + qb.x_ * 0.5f,
                   qa.y_ * 0.5f + qb.y_ * 0.5f,
                   qa.z_ * 0.5f + qb.z_ * 0.5f,
                   qa.w_ * 0.5f + qb.w_ * 0.5f);
            }
            double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
            double ratioB = sin(t * halfTheta) / sinHalfTheta;

            // Calculate the final values
            return QuaternionTemplate<T>(qa.x_ * ratioA + qb.x_ * ratioB,
               qa.y_ * ratioA + qb.y_ * ratioB,
               qa.z_ * ratioA + qb.z_ * ratioB,
               qa.w_ * ratioA + qb.w_ * ratioB);
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

        const T w() const
        {
            return w_;
        }

        void setX(T const& r)
        {
            x_ = r.x();
        }

        void setY(T const& r)
        {
            y_ = r.y();
        }

        void setZ(T const& r)
        {
            z_ = r.z();
        }

        void setW(T const& r)
        {
            w_ = r.w();
        }

    private:
        T x_;
        T y_;
        T z_;
        T w_;
    };
}

#endif
