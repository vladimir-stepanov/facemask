/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/CameraComponentPrivate.hpp"
#include "Grapevine/Math/Math.hpp"
#include "Grapevine/SceneExtensions/CameraComponent.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            CameraComponentPrivate::CameraComponentPrivate(Canvas const& canvas, float fieldOfViewY, float nearPlane, float farPlane):
                AbstractComponentPrivateTemplate<CameraComponentPrivate>("CameraComponent"),
                canvas_(&canvas),
                nearPlane_(nearPlane),
                farPlane_(farPlane),
                fieldOfViewY_(fieldOfViewY)
            {
            }

            CameraComponentPrivate::~CameraComponentPrivate()
            {
            }

            Matrix4 CameraComponentPrivate::view(Vector3 const& location, Matrix4 const& rotation) const
            {
                // Create a vector pointing the direction the camera is facing.
                Vector3 transformedReference = Vector::transform(CameraComponent::referenceLook, rotation);

                // Calculate the position the camera is looking at.
                Vector3 cameraLookAt = location + transformedReference;

                // Set up the view matrix and projection matrix.
                return Matrix::lookAt(location, cameraLookAt, Vector::transform(CameraComponent::referenceUp, rotation));
            }

            Matrix4 CameraComponentPrivate::projection() const
            {
                Matrix4 r;
                if (canvas_ == NULL)
                {
                    throw Exception("Camera::projection() needs canvas to know aspect ratio");
                }
                else
                {
                    r = Matrix::perspective(fieldOfViewY_,
                        (float)canvas_->width() / (float)canvas_->height(),
                        nearPlane_,
                        farPlane_);
                }

                return r;
            }

            Matrix4 CameraComponentPrivate::viewProjection(Vector3 const& location, Matrix4 const& rotation) const
            {
                return projection() * view(location, rotation);
            }

            /**
             * @returns camera's direction if no canvas is set.
             */
            Ray CameraComponentPrivate::mouseCoordinatesToRay(int x, int y, Vector3 const& location, Matrix4 const& rotation)
            {
                if (canvas_ == NULL)
                {
                    Vector3 lookDirection = Vector::transform(CameraComponent::referenceLook, rotation);
                    return Ray(location, lookDirection);
                }

                float w = (float)canvas_->width();
                float h = (float)canvas_->height();

                // y:       /|
                //        _/ |a
                //      _/   |
                //    _/     * <-uY
                //   /)fovy/2|
                //  /__)_____|
                //     ref.z
                //
                float a = tan((fieldOfViewY_ / 2.0f) / 180.0f * Math::Pi) * CameraComponent::referenceLook.z();
                float uY = (y - h / 2) / (h / 2) * a;

                // x:
                float b = w / h * a;
                float uX = -(x - w / 2) / (w / 2) * b;

                // Look direction vector
                Vector3 u(uX, uY, CameraComponent::referenceLook.z());
                u.normalize();
                Vector3 lookDirection = Vector::transform(u, rotation);

                return Ray(location, lookDirection);
            }

            Ray CameraComponentPrivate::lookAt(Vector3 const& location, Matrix4 const& rotation)
            {
                Vector3 transformedReference = Vector::transform(CameraComponent::referenceLook, rotation);
                return Ray(location, transformedReference);
            }

            float CameraComponentPrivate::fieldOfViewY()
            {
                return fieldOfViewY_;
            }

            float CameraComponentPrivate::aspectRatio()
            {
                return (float)canvas_->width() / (float)canvas_->height();
            }

            /**
             * Camera's up direction in world coordinates.
             *
             * @return       Camera's up unit vector
             */
            Vector3 CameraComponentPrivate::up(Matrix4 const& rotation)
            {
                return Vector::transform(CameraComponent::referenceUp, rotation);
            }

            /**
             * Camera's right direction in world coordinates.
             *
             * @return       Camera's right unit vector
             */
            Vector3 CameraComponentPrivate::right(Matrix4 const& rotation)
            {
                return Vector::transform(CameraComponent::referenceRight, rotation);
            }
        }
    }
}
