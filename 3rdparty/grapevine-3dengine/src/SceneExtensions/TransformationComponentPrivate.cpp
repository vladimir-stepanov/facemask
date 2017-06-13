/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include <Grapevine/Animation/AnimationManager.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            TransformationComponentPrivate::TransformationComponentPrivate(Vector3 const& location):
                AbstractComponentPrivateTemplate<TransformationComponentPrivate>("TransformationComponent"),
                controlPoint_(location),
                opacityCumulativiness_(true),
                worldOpacity_(1.0f)
            {
            }

            TransformationComponentPrivate::TransformationComponentPrivate(ControlPoint const& controlPoint):
                AbstractComponentPrivateTemplate<TransformationComponentPrivate>("TransformationComponent"),
                controlPoint_(controlPoint),
                opacityCumulativiness_(true),
                worldOpacity_(1.0f)
            {
            }

            TransformationComponentPrivate::~TransformationComponentPrivate()
            {
            }

            void TransformationComponentPrivate::setLocation(Vector3 const& location)
            {
                controlPoint_.setLocation(location);
            }

            Vector3 const& TransformationComponentPrivate::location() const
            {
                return controlPoint_.location();
            }

            void TransformationComponentPrivate::setRotation(Vector3 const& axis, float angle)
            {
                controlPoint_.setRotation(Matrix::rotate(angle, axis.x(), axis.y(), axis.z()));
            }

            void TransformationComponentPrivate::setRotation(Matrix4 const& rotation)
            {
                controlPoint_.setRotation(rotation);
            }

            void TransformationComponentPrivate::rotate(Matrix4 const& matrix)
            {
                controlPoint_.setRotation(controlPoint_.rotation() * matrix);
            }

            Matrix4 TransformationComponentPrivate::rotation() const
            {
                return controlPoint_.rotation();
            }

            void TransformationComponentPrivate::move(Vector3 const& amount)
            {
                controlPoint_.move(amount);
            }

            void TransformationComponentPrivate::setScale(Vector3 const& scalingFactor)
            {
                controlPoint_.setScale(scalingFactor);
            }

            Vector3 TransformationComponentPrivate::scale() const
            {
                return controlPoint_.scale();
            }

            float TransformationComponentPrivate::largestScaleFactor() const
            {
                return controlPoint_.largestScaleFactor();
            }

            void TransformationComponentPrivate::setOpacity(float opacity)
            {
                controlPoint_.setOpacity(opacity);
            }

            float TransformationComponentPrivate::opacity() const
            {
                return controlPoint_.opacity();
            }

            void TransformationComponentPrivate::setOpacityCumulativiness(bool value)
            {
                opacityCumulativiness_ = value;
            }

            bool TransformationComponentPrivate::opacityCumulativiness() const
            {
                return opacityCumulativiness_;
            }

            Matrix4 TransformationComponentPrivate::transformation() const
            {
                return controlPoint_.transformation();
            }

            void TransformationComponentPrivate::setWorldTransformation(const Matrix4 &worldTransformation)
            {
                worldTransformation_ = worldTransformation;
                //AnimationManager::setSceneDirty();
            }

            const Matrix4 &TransformationComponentPrivate::worldTransformation()
            {
                return worldTransformation_;
            }

            void TransformationComponentPrivate::setWorldOpacity(float value)
            {
                worldOpacity_ = value;
            }

            float TransformationComponentPrivate::worldOpacity() const
            {
                return worldOpacity_;
            }

            void TransformationComponentPrivate::setWorldScaling(Vector3 const& value)
            {
                worldScaling_ = value;
            }

            Vector3 TransformationComponentPrivate::worldScaling() const
            {
                return worldScaling_;
            }

            float TransformationComponentPrivate::largestWorldScaleFactor() const
            {
                float r = worldScaling_.x();
                r = worldScaling_.y() > r ? worldScaling_.y() : r;
                r = worldScaling_.z() > r ? worldScaling_.z() : r;
                return r;
            }

            ControlPoint TransformationComponentPrivate::controlPoint() const
            {
                return controlPoint_;
            }
        }
    }
}
