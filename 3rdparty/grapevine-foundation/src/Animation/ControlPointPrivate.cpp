/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "ControlPointPrivate.hpp"
#include "Grapevine/Animation/AnimationManager.hpp"

namespace Grapevine
{
    ControlPointPrivate::ControlPointPrivate(Vector3 const& location, Matrix4 const& rotation, Vector3 const& scaling, float opacity):
        Referenced(),
        location_(location),
        rotation_(rotation),
        scaling_(scaling),
        opacity_(opacity)
    {
    }

    ControlPointPrivate::~ControlPointPrivate()
    {
    }

    void ControlPointPrivate::setOpacity(float opacity)
    {
        opacity_ = opacity;
        AnimationManager::setSceneDirty();
    }

    float ControlPointPrivate::opacity() const
    {
        return opacity_;
    }

    void ControlPointPrivate::setLocation(Vector3 const& location)
    {
        location_ = location;
        AnimationManager::setSceneDirty();
    }

    Vector3 const& ControlPointPrivate::location() const
    {
        return location_;
    }

    void ControlPointPrivate::move(Vector3 const& amount)
    {
        location_ += amount;
        AnimationManager::setSceneDirty();
    }

    Matrix4 const& ControlPointPrivate::rotation() const
    {
        return rotation_;
    }

    void ControlPointPrivate::setRotation(Vector3 const& axis, float angle)
    {
        rotation_ = Matrix::rotate(angle, axis.x(), axis.y(), axis.z());
        AnimationManager::setSceneDirty();
    }

    void ControlPointPrivate::setRotation(Matrix4 const& rotation)
    {
        rotation_ = rotation;
        AnimationManager::setSceneDirty();
    }

    void ControlPointPrivate::rotate(Matrix4 const& matrix)
    {
        rotation_ *= matrix;
    }

    Vector3 const& ControlPointPrivate::scale() const
    {
        return scaling_;
    }

    float ControlPointPrivate::largestScaleFactor() const
    {
        float r = scaling_.x();
        r = scaling_.y() > r ? scaling_.y() : r;
        r = scaling_.z() > r ? scaling_.z() : r;
        return r;
    }

    void ControlPointPrivate::setScale(Vector3 const& scaling)
    {
        scaling_ = scaling;
        AnimationManager::setSceneDirty();
    }

    Matrix4 ControlPointPrivate::transformation() const
    {
        Matrix4 translate(Matrix::translate(location_.x(), location_.y(), location_.z()));
        return translate * rotation_ * Matrix::scale(scaling_.x(), scaling_.y(), scaling_.z());
    }
}
