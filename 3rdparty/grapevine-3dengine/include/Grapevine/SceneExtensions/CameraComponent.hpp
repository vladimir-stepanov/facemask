/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_CAMERACOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_CAMERACOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Ray.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            /**
             * @class CameraComponent
             *
             * CameraComponent enables Graph drawing to Canvas.
             *
             * @todo
             *      -TODO: Polish canvas_; // weak pointer
             */
            class CameraComponent: public Component
            {
            public:
                CLASS_COPYABLE(CameraComponent)

                static const Vector3 referenceLook;
                static const Vector3 referenceUp;
                static const Vector3 referenceRight;

                /**
                 * Creates Camera with given parameters.
                 *
                 * @param [in]  canvas          Canvas to draw to.
                 * @param [in]  fieldOfViewY    Field of view in Y direction.
                 * @param [in]  nearPlane       Near frustum place
                 * @param [in]  farPlane        Far frustum place
                 */
                CameraComponent(Canvas const& canvas,
                    float fieldOfViewY = 90.0f,
                    float nearPlane = 1.0f,
                    float farPlane = 10000000.0f);

                /**
                 * Releases resources.
                 */
                virtual ~CameraComponent();

                /**
                 * Camera view projection matrix
                 *
                 * @return       View projection matrix.
                 */
                Matrix4 viewProjection(Vector3 const& location, Matrix4 const& rotation) const;

                /**
                 * Sets new near and far plane.
                 *
                 * @param [in]  nearPlane       New near plane distance.
                 * @param [in]  farPlane        New far plane distance.
                 */
                void setPlanes(float nearPlane, float farPlane);

                /**
                 * Converts pixel coordinates to ray.
                 *
                 * @note Canvas pixel size used as reference.
                 *
                 * @return      Ray in world coordinates. Starting point is
                 *              camera's location.
                 */
                Ray mouseCoordinatesToRay(int x, int y, Vector3 const& location, Matrix4 const& rotation);

                /**
                 * Camera look direction
                 *
                 * @return      Ray in world coordinates. Starting point is
                 *              camera's location.
                 */
                Ray lookAt(Vector3 const& location, Matrix4 const& rotation);

                /**
                 * Camera's fovy
                 *
                 * @return       Camera's fovy in degrees
                 */
                float fieldOfViewY();

                /**
                 * Camera's aspectRatio
                 *
                 * @return       Camera's aspectRatio
                 */
                float aspectRatio();

                /**
                 * Camera's up direction in world coordinates.
                 *
                 * @return       Camera's up unit vector
                 */
                Vector3 up(Matrix4 const& rotation);

                /**
                 * Camera's right direction in world coordinates.
                 *
                 * @return       Camera's right unit vector
                 */
                Vector3 right(Matrix4 const& rotation);

            private:
                class CameraComponentPrivate* cameraComponentPrivate_;
            };
        }
    }
}

#endif
