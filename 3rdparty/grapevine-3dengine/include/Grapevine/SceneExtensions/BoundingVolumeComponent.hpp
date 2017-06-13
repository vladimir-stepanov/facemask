/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_BOUNDINGVOLUMECOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_BOUNDINGVOLUMECOMPONENT

#include <Grapevine/Math/Ray.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Box.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <vector>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class BoundingVolumeComponent : public Component
            {
                public:
                    BoundingVolumeComponent();
                    BoundingVolumeComponent(const Vector3 &min, const Vector3 &max);
                    BoundingVolumeComponent(const Vector2 &min, const Vector2 &max);
                    BoundingVolumeComponent(const Plane &plane);
                    BoundingVolumeComponent(const Sphere &sphere);

                    /* set rect */
                    void setBoundingVolume(const Vector2 &min, const Vector2 &max);

                    /* set plane */
                    void setBoundingVolume(const Plane &plane);

                    /* set sphere */
                    void setBoundingVolume(const Sphere &sphere);

                    /* set box */
                    void setBoundingVolume(const Vector3 &min, const Vector3 &max);

                    /* get intersections on ray */
                    std::vector<float> intersections(Ray &transformedRay);
            };
        }
    }
}

#endif
