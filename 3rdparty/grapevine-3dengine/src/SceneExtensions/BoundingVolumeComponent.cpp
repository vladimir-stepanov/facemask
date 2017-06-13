/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/BoundingVolumeComponent.hpp"
#include "Grapevine/SceneExtensions/BoundingVolumeComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            BoundingVolumeComponent::BoundingVolumeComponent()
            : Component(new BoundingVolumeComponentPrivate())
            {
            }


            BoundingVolumeComponent::BoundingVolumeComponent(const Vector3 &min, const Vector3 &max)
            : Component(new BoundingVolumeComponentPrivate(min, max))
            {
            }

            BoundingVolumeComponent::BoundingVolumeComponent(const Vector2 &min, const Vector2 &max)
            : Component(new BoundingVolumeComponentPrivate())
            {
                setBoundingVolume(min,max);
            }

            BoundingVolumeComponent::BoundingVolumeComponent(const Plane &plane)
            : Component(new BoundingVolumeComponentPrivate())
            {
                setBoundingVolume(plane);
            }

            BoundingVolumeComponent::BoundingVolumeComponent(const Sphere &sphere)
            : Component(new BoundingVolumeComponentPrivate())
            {
                setBoundingVolume(sphere);
            }

            void BoundingVolumeComponent::setBoundingVolume(const Vector2 &min, const Vector2 &max)
            {
                static_cast<BoundingVolumeComponentPrivate *>(private_.pointer())->setBoundingVolume(min, max);
            }

            void BoundingVolumeComponent::setBoundingVolume(const Plane &plane)
            {
                static_cast<BoundingVolumeComponentPrivate *>(private_.pointer())->setBoundingVolume(plane);
            }

            void BoundingVolumeComponent::setBoundingVolume(const Sphere &sphere)
            {
                static_cast<BoundingVolumeComponentPrivate *>(private_.pointer())->setBoundingVolume(sphere);
            }

            void BoundingVolumeComponent::setBoundingVolume(const Vector3 &min, const Vector3 &max)
            {
                static_cast<BoundingVolumeComponentPrivate *>(private_.pointer())->setBoundingVolume(min, max);
            }

            std::vector<float> BoundingVolumeComponent::intersections(Ray &ray)
            {
                return static_cast<BoundingVolumeComponentPrivate *>(private_.pointer())->intersections(ray);
            }
        }
    }
}

