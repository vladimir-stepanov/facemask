/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_TRANSFORMATIONCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_TRANSFORMATIONCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Animation/ControlPoint.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class TransformationComponent: public Component
            {
            public:
                CLASS_COPYABLE(TransformationComponent)

                TransformationComponent(Vector3 const& location = Vector3());
                TransformationComponent(ControlPoint const& controlPoint);

                virtual ~TransformationComponent();

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

                void setOpacity(float opacity);
                float opacity() const;
                void setOpacityCumulativiness(bool value);
                bool opacityCumulativiness() const;

                Matrix4 transformation() const;

                ControlPoint controlPoint() const;
            private:
                class TransformationComponentPrivate* transformationComponentPrivate_;
            };
        }
    }
}

#endif
