/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/TransformationComponent.hpp"
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            TransformationComponent::TransformationComponent(Vector3 const& location):
                Component(new TransformationComponentPrivate(location))
            {
                transformationComponentPrivate_ = dynamic_cast<TransformationComponentPrivate*>(private_.pointer());
            }

            TransformationComponent::TransformationComponent(ControlPoint const& controlPoint):
                Component(new TransformationComponentPrivate(controlPoint))
            {
                transformationComponentPrivate_ = dynamic_cast<TransformationComponentPrivate*>(private_.pointer());
            }

            TransformationComponent::TransformationComponent(TransformationComponent const &r)
            : Component(r)
            {
                transformationComponentPrivate_ = r.transformationComponentPrivate_;
            }

            TransformationComponent::~TransformationComponent()
            {
            }

            TransformationComponent& TransformationComponent::operator=(TransformationComponent const& r)
            {
                Component::operator=(r);

                transformationComponentPrivate_ = r.transformationComponentPrivate_;

                return *this;
            }

            void TransformationComponent::setLocation(Vector3 const& location)
            {
                transformationComponentPrivate_->setLocation(location);
            }

            Vector3 const& TransformationComponent::location() const
            {
                return transformationComponentPrivate_->location();
            }

            void TransformationComponent::setRotation(Vector3 const& axis, float angle)
            {
                transformationComponentPrivate_->setRotation(axis, angle);
            }

            void TransformationComponent::setRotation(Matrix4 const& rotation)
            {
                transformationComponentPrivate_->setRotation(rotation);
            }

            void TransformationComponent::rotate(Matrix4 const& matrix)
            {
                transformationComponentPrivate_->rotate(matrix);
            }

            Matrix4 TransformationComponent::rotation() const
            {
                return transformationComponentPrivate_->rotation();
            }

            void TransformationComponent::move(Vector3 const& amount)
            {
                transformationComponentPrivate_->move(amount);
            }

            void TransformationComponent::setScale(Vector3 const& scalingFactor)
            {
                transformationComponentPrivate_->setScale(scalingFactor);
            }

            Vector3 TransformationComponent::scale() const
            {
                return transformationComponentPrivate_->scale();
            }

            float TransformationComponent::largestScaleFactor() const
            {
                return transformationComponentPrivate_->largestScaleFactor();
            }

            void TransformationComponent::setOpacity(float opacity)
            {
                transformationComponentPrivate_->setOpacity(opacity);
            }

            float TransformationComponent::opacity() const
            {
                return transformationComponentPrivate_->opacity();
            }

            void TransformationComponent::setOpacityCumulativiness(bool value)
            {
                transformationComponentPrivate_->setOpacityCumulativiness(value);
            }

            bool TransformationComponent::opacityCumulativiness() const
            {
                return transformationComponentPrivate_->opacityCumulativiness();
            }

            Matrix4 TransformationComponent::transformation() const
            {
                return transformationComponentPrivate_->transformation();
            }

            ControlPoint TransformationComponent::controlPoint() const
            {
                return transformationComponentPrivate_->controlPoint();
            }
        }
    }
}
