/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Animation/ControlPoint.hpp"
#include "ControlPointPrivate.hpp"

namespace Grapevine
{
    ControlPoint::ControlPoint(Vector3 const& location, Matrix4 const& rotation, Vector3 const& scale,float opacity):
        private_(ReferencedPointer<ControlPointPrivate>(new ControlPointPrivate(location, rotation, scale,opacity)))
    {
    }

    ControlPoint::ControlPoint(ControlPoint const& r):
        private_(r.private_)
    {
    }

    ControlPoint::~ControlPoint()
    {
    }

    ControlPoint& ControlPoint::operator=(ControlPoint const& r)
    {
        if (this != &r)
        {
            private_ = r.private_;
        }
        return *this;
    }

    bool ControlPoint::operator==(const ControlPoint& r) const
    {
        return r.private_.pointer() == private_.pointer();
    }

    void ControlPoint::setOpacity(float opacity)
    {
        private_.pointer()->setOpacity(opacity);
    }

    float ControlPoint::opacity() const
    {
        return private_.pointer()->opacity();
    }

    Vector3 const& ControlPoint::location() const
    {
        return private_.pointer()->location();
    }

    void ControlPoint::setLocation(Vector3 const& location)
    {
        private_.pointer()->setLocation(location);
    }

    void ControlPoint::move(Vector3 const& amount)
    {
        private_.pointer()->move(amount);
    }

    Matrix4 const& ControlPoint::rotation() const
    {
        return private_.pointer()->rotation();
    }

    void ControlPoint::setRotation(Vector3 const& axis, float angle)
    {
        private_.pointer()->setRotation(axis, angle);
    }

    void ControlPoint::setRotation(Matrix4 const& rotation)
    {
        private_.pointer()->setRotation(rotation);
    }

    void ControlPoint::rotate(Matrix4 const& matrix)
    {
        private_.pointer()->rotate(matrix);
    }

    Vector3 const& ControlPoint::scale() const
    {
        return private_.pointer()->scale();
    }

    float ControlPoint::largestScaleFactor() const
    {
        return private_.pointer()->largestScaleFactor();
    }

    void ControlPoint::setScale(Vector3 const& scaling)
    {
        private_.pointer()->setScale(scaling);
    }

    Matrix4 ControlPoint::transformation() const
    {
        return private_.pointer()->transformation();
    }
}
