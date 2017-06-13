/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_TRANSFORMATIONCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_TRANSFORMATIONCOMPONENTPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Animation/ControlPoint.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class TransformationComponentPrivate:
                public AbstractComponentPrivateTemplate<TransformationComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(TransformationComponentPrivate)

                TransformationComponentPrivate(Vector3 const& location = Vector3());
                TransformationComponentPrivate(ControlPoint const& controlPoint);

                virtual ~TransformationComponentPrivate();

                Vector3 const& location() const;
                void setLocation(Vector3 const& location);
                void move(Vector3 const& amount);

                Matrix4 rotation() const;
                void setRotation(Vector3 const& axis, float angle);
                void setRotation(Matrix4 const& rotation);
                void rotate(Matrix4 const& matrix);

                Vector3 scale() const;
                float largestScaleFactor() const;
                void setScale(Vector3 const& scale);

                Matrix4 transformation() const;

                void setOpacity(float opacity);
                float opacity() const;
                void setOpacityCumulativiness(bool value);
                bool opacityCumulativiness() const;

                void setWorldTransformation(const Matrix4 &worldTransformation);
                const Matrix4 &worldTransformation();
                void setWorldOpacity(float);
                float worldOpacity() const;
                void setWorldScaling(Vector3 const&);
                Vector3 worldScaling() const;
                float largestWorldScaleFactor() const;

                ControlPoint controlPoint() const;

            protected:
                ControlPoint controlPoint_;
                Matrix4 worldTransformation_;
                float worldOpacity_;
                Vector3 worldScaling_;
                bool opacityCumulativiness_;
            };
        }
    }
}

#endif
