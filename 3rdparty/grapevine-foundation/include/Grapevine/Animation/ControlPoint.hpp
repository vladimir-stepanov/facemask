/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_CONTROLPOINT
#define GRAPEVINE_ANIMATION_CONTROLPOINT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Math/Vector.hpp>

namespace Grapevine
{
    class ControlPoint
    {
    public:
        CLASS_COPYABLE(ControlPoint)

        ControlPoint(Vector3 const& location = Vector3(), Matrix4 const& rotation = Matrix4(), Vector3 const& scale = Vector3(1.0f, 1.0f, 1.0f), float opacity = 1.0f);
        virtual ~ControlPoint();
        bool operator==(const ControlPoint& r) const;
        Vector3 const& location() const;
        void setLocation(Vector3 const& location);
        void move(Vector3 const& amount);
        Matrix4 const& rotation() const;
        void setRotation(Vector3 const& axis, float angle);
        void setRotation(Matrix4 const& rotation);
        void rotate(Matrix4 const& matrix);
        Vector3 const& scale() const;
        float largestScaleFactor() const;
        void setScale(Vector3 const& scalingFactor);
        Matrix4 transformation() const;
        void setOpacity(float opacity);
        float opacity() const;

    private:
        ReferencedPointer<class ControlPointPrivate> private_;
    };
}

#endif
