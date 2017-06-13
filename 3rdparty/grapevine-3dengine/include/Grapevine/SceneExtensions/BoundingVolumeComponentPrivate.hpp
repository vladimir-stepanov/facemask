/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_BOUNDINGVOLUMECOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_BOUNDINGVOLUMECOMPONENTPRIVATE

#include "BoundingVolumeComponent.hpp"
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            enum BoundingVolumeType
            {
                NotSet,
                RectType,
                PlaneType,
                SphereType,
                BoxType
            };

            struct BoundingRect
            {
                BoundingRect()
                {}

                BoundingRect(Grapevine::Vector2 min, Grapevine::Vector2 max)
                : min_(min), max_(max)
                {}

                Grapevine::Vector2 min_;
                Grapevine::Vector2 max_;
            };

            class BoundingVolumeComponentPrivate: public AbstractComponentPrivateTemplate<BoundingVolumeComponentPrivate>
            {
                friend class BoundingVolumeComponent;
                public:
                    BoundingVolumeComponentPrivate();
                    BoundingVolumeComponentPrivate(const Vector3 &min, const Vector3 &max);

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

                    Vector3 min() const;
                    Vector3 max() const;

                protected:
                private:
                    void setRectIntersections(std::vector<float> &tValues, Ray &transformedRay);
                    void setSphereIntersections(std::vector<float> &tValues, Ray &objectSpaceRay);
                    void setPlaneIntersections(std::vector<float> &tValues, Ray &objectSpaceRay);
                    void setBoxIntersections(std::vector<float> &tValues, Ray &objectSpaceRay);

                    BoundingRect rectArea_;
                    Plane plane_;
                    Sphere sphere_;
                    Box box_;
                    BoundingVolumeType activeType_;
            };
        }
    }
}

#endif
