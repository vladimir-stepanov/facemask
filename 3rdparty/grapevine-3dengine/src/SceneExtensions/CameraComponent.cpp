/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/CameraComponent.hpp"
#include "Grapevine/SceneExtensions/CameraComponentPrivate.hpp"
#include <Grapevine/Math/Math.hpp>
#include <Grapevine/Core/Exception.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            // Initial camera look direction
            Vector3 const CameraComponent::referenceLook = Vector3(0.0f, 0.0f, -1.0f);
            Vector3 const CameraComponent::referenceUp = Vector::yAxis;
            Vector3 const CameraComponent::referenceRight = Vector::xAxis;

            CameraComponent::CameraComponent(Canvas const& canvas, float fieldOfViewY, float nearPlane, float farPlane):
                Component(new CameraComponentPrivate(canvas, fieldOfViewY, nearPlane, farPlane))
            {
                cameraComponentPrivate_ = dynamic_cast<CameraComponentPrivate*>(private_.pointer());
            }

            CameraComponent::CameraComponent(CameraComponent const &r)
            : Component(r)
            {
                cameraComponentPrivate_ = r.cameraComponentPrivate_;
            }

            CameraComponent::~CameraComponent()
            {
            }

            CameraComponent &CameraComponent::operator=(CameraComponent const &r)
            {
                Component::operator=(r);
                cameraComponentPrivate_ = r.cameraComponentPrivate_;
                return *this;
            }

            Matrix4 CameraComponent::viewProjection(Vector3 const& location, Matrix4 const& rotation) const
            {
                return cameraComponentPrivate_->viewProjection(location, rotation);
            }

            Ray CameraComponent::mouseCoordinatesToRay(int x, int y, Vector3 const& location, Matrix4 const& rotation)
            {
                return cameraComponentPrivate_->mouseCoordinatesToRay(x, y, location, rotation);
            }

            Ray CameraComponent::lookAt(Vector3 const& location, Matrix4 const& rotation)
            {
                return cameraComponentPrivate_->lookAt(location, rotation);
            }

            float CameraComponent::fieldOfViewY()
            {
                return cameraComponentPrivate_->fieldOfViewY();
            }

            float CameraComponent::aspectRatio()
            {
                return cameraComponentPrivate_->aspectRatio();
            }

            Vector3 CameraComponent::up(Matrix4 const& rotation)
            {
                return cameraComponentPrivate_->up(rotation);
            }

            Vector3 CameraComponent::right(Matrix4 const& rotation)
            {
                return cameraComponentPrivate_->right(rotation);
            }
        }
    }
}
