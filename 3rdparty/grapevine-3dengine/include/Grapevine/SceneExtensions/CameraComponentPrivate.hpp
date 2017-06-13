/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_CAMERACOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_CAMERACOMPONENTPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Ray.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class CameraComponentPrivate: public AbstractComponentPrivateTemplate<CameraComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(CameraComponentPrivate)

                CameraComponentPrivate(Canvas const& canvas,
                    float fieldOfViewY = 90.0f,
                    float nearPlane = 1.0f,
                    float farPlane = 1024.0f);

                ~CameraComponentPrivate();
                Matrix4 viewProjection(Vector3 const& location, Matrix4 const& rotation) const;
                void setPlanes(float nearPlane, float farPlane);
                Ray mouseCoordinatesToRay(int x, int y, Vector3 const& location, Matrix4 const& rotation);
                Ray lookAt(Vector3 const& location, Matrix4 const& rotation);
                float fieldOfViewY();
                float aspectRatio();
                Vector3 up(Matrix4 const& rotation);
                Vector3 right(Matrix4 const& rotation);

            private:
                Matrix4 projection() const;
                Matrix4 view(Vector3 const& location, Matrix4 const& rotation) const;
                Canvas const* canvas_; // weak pointer
                float nearPlane_;
                float farPlane_;
                float fieldOfViewY_;
            };
        }
    }
}

#endif
