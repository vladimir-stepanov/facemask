/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <cassert>
#include <limits>
#include <algorithm>

#include <Grapevine/Math/Ray.hpp>
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Matrix.hpp>
#include <Grapevine/Scene/AbstractComponentPrivate.hpp>
#include "Grapevine/SceneExtensions/BoundingVolumeComponentPrivate.hpp"

using namespace Grapevine;

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            namespace
            {
                void rectIntersections(std::vector<float> &tValues, Ray &objectSpaceRay, const Vector2 &min, const Vector2 &max, float major, int axisMajor)
                {
                    float divider = objectSpaceRay.unit()[axisMajor];

                    if(divider == 0.f)
                    {
                        return;
                    }

                    float t = (major - objectSpaceRay.origin()[axisMajor]) / divider;

                    const Vector3 &objectSpacePoint = objectSpaceRay.origin() + objectSpaceRay.unit() * t;

                    if(objectSpacePoint.x() >= min.x() && objectSpacePoint.x() < max.x() &&
                       objectSpacePoint.y() >= min.y() && objectSpacePoint.y() < max.y())
                    {
                        tValues.push_back(t);
                    }
                }

            }

            BoundingVolumeComponentPrivate::BoundingVolumeComponentPrivate():
                AbstractComponentPrivateTemplate<BoundingVolumeComponentPrivate>("BoundingVolumeComponent"),
                plane_(Vector3(0,0,0), Vector3(0,0,0)),
                activeType_(NotSet)
            {
            }

            BoundingVolumeComponentPrivate::BoundingVolumeComponentPrivate(const Vector3 &min, const Vector3 &max):
                AbstractComponentPrivateTemplate<BoundingVolumeComponentPrivate>("BoundingVolumeComponent"),
                plane_(Vector3(0,0,0), Vector3(0,0,0)),
                box_(min, max),
                activeType_(BoxType)
            {
            }

            void BoundingVolumeComponentPrivate::setBoundingVolume(const Grapevine::Vector2 &min, const Grapevine::Vector2 &max)
            {
                rectArea_ = BoundingRect(min,max);
                activeType_ = RectType;
            }

            void BoundingVolumeComponentPrivate::setBoundingVolume(const Plane &plane)
            {
                plane_ = plane;
                activeType_ = PlaneType;
            }

            void BoundingVolumeComponentPrivate::setBoundingVolume(const Sphere &sphere)
            {
                sphere_ = sphere;
                activeType_ = SphereType;
            }

            void BoundingVolumeComponentPrivate::setBoundingVolume(const Vector3 &min, const Vector3 &max)
            {
                box_ = Box(min, max);
                activeType_ = BoxType;
            }

            /* get intersections on ray */
            std::vector<float> BoundingVolumeComponentPrivate::intersections(Ray &transformedRay)
            {
                std::vector<float> tValues;

                switch(activeType_)
                {
                    case RectType:
                        setRectIntersections(tValues, transformedRay);
                        break;
                    case SphereType:
                        setSphereIntersections(tValues, transformedRay);
                        break;
                    case PlaneType:
                        setPlaneIntersections(tValues, transformedRay);
                        break;
                    case BoxType:
                        setBoxIntersections(tValues, transformedRay);
                        break;
                    default:
                        break;
                }

                return tValues;
            }


            void BoundingVolumeComponentPrivate::setRectIntersections(std::vector<float> &tValues, Ray &objectSpaceRay)
            {
                rectIntersections(tValues, objectSpaceRay, rectArea_.min_, rectArea_.max_, 0, 2);
            }

            void BoundingVolumeComponentPrivate::setPlaneIntersections(std::vector<float> &tValues, Ray &objectSpaceRay)
            {
                RayPlaneIntersection intersection = objectSpaceRay.intersection(plane_);

                switch(intersection.type())
                {
                    case RayPlaneIntersection::Type::PointIntersection:
                        {
                            float t = objectSpaceRay.distance(intersection.p());
                            tValues.push_back(t);
                        }
                        break;
                    default:
                        break;
                }
            }

            void BoundingVolumeComponentPrivate::setSphereIntersections(std::vector<float> &tValues, Ray &objectSpaceRay)
            {
                RaySphereIntersection intersection = objectSpaceRay.intersection(sphere_);

                switch(intersection.type())
                {
                    case RaySphereIntersection::Type::PointIntersection:
                        {
                            float t = objectSpaceRay.distance(intersection.p0());
                            tValues.push_back(t);
                        }
                        break;
                    case RaySphereIntersection::Type::TwoPointIntersection:
                        {
                            float t0 = objectSpaceRay.distance(intersection.p0());
                            float t1 = objectSpaceRay.distance(intersection.p0());
                            if(t1 > t0)
                            {
                                tValues.push_back(t0);
                                tValues.push_back(t1);
                            }
                            else
                            {
                                tValues.push_back(t1);
                                tValues.push_back(t0);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            void BoundingVolumeComponentPrivate::setBoxIntersections(std::vector<float> &tValues, Ray &objectSpaceRay)
            {
                Vector3 max = box_.max();
                Vector3 min = box_.min();

                // Top
                rectIntersections(tValues, objectSpaceRay, Vector2(min.x(), min.z()), Vector2(max.x(), max.z()), max.y(), 1);

                // Front
                rectIntersections(tValues, objectSpaceRay, Vector2(min.x(), min.y()), Vector2(max.x(), max.y()), max.z(), 2);

                // Right
                rectIntersections(tValues, objectSpaceRay, Vector2(min.z(), min.y()), Vector2(max.z(), max.y()), min.x(), 0);

                // Bottom
                rectIntersections(tValues, objectSpaceRay, Vector2(min.x(), min.z()), Vector2(max.x(), max.z()), min.y(), 1);

                // Back
                rectIntersections(tValues, objectSpaceRay, Vector2(min.x(), min.y()), Vector2(max.x(), max.y()), min.z(), 2);

                // Left
                rectIntersections(tValues, objectSpaceRay, Vector2(min.z(), min.y()), Vector2(max.z(), max.y()), min.x(), 0);


                std::sort(tValues.begin(), tValues.end());
            }

            Vector3 BoundingVolumeComponentPrivate::min() const
            {
                switch(activeType_)
                {
                    case RectType:
                        return Vector3(rectArea_.min_.x(), rectArea_.min_.y(), 0.f);
                        break;
                    case SphereType:
                        return sphere_.center() - sphere_.radius();
                        break;
                    case PlaneType:
                        return Vector3(std::abs(plane_.normal().x()) == 1.f ? 0.f : std::numeric_limits<float>::min(),
                                       std::abs(plane_.normal().y()) == 1.f ? 0.f : std::numeric_limits<float>::min(),
                                       std::abs(plane_.normal().z()) == 1.f ? 0.f : std::numeric_limits<float>::min());
                        break;
                    case BoxType:
                        return box_.min();
                        break;
                    default:
                        assert(!"BUG in BoundingVolumeComponentPrivate");
                        return Vector3(); /* -Werror=return-type */
                }
            }

            Vector3 BoundingVolumeComponentPrivate::max() const
            {
                switch(activeType_)
                {
                    case RectType:
                        return Vector3(rectArea_.max_.x(), rectArea_.max_.y(), 0.f);
                        break;
                    case SphereType:
                        return sphere_.center() + sphere_.radius();
                        break;
                    case PlaneType:
                        return Vector3(std::abs(plane_.normal().x()) == 1.f ? 0.f : std::numeric_limits<float>::max(),
                                       std::abs(plane_.normal().y()) == 1.f ? 0.f : std::numeric_limits<float>::max(),
                                       std::abs(plane_.normal().z()) == 1.f ? 0.f : std::numeric_limits<float>::max());
                        break;
                    case BoxType:
                        return box_.max();
                        break;
                    default:
                        assert(!"BUG in BoundingVolumeComponentPrivate");
                        return Vector3(); /* -Werror=return-type */
                }
            }
        }
    }
}
