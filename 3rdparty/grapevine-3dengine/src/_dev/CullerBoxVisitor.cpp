/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "CullerBoxVisitor.hpp"
#include <Grapevine/Model/MeshResource.hpp>
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/CameraComponentPrivate.hpp"
#include <Grapevine/Scene/NodePrivate.hpp>
#include <stdexcept>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            CullerBoxVisitor::CullerBoxVisitor(Node& cameraNode, Vector3 const& cullingBoxOrigo, Vector3 const& cullingBoxAxis) throw (Exception):
                AbstractTreeVisitor("CullerBoxVisitor"),
                cullingBoxOrigo_(cullingBoxOrigo),
                cullingBoxAxis_(cullingBoxAxis),
                cameraNode_(cameraNode),
                fieldOfViewRatio_(0.0f),
                cameraAbsoluteLocation_(Vector::origo),
                cameraLookAt_(Ray(Vector::origo, Vector::origo)),
                cameraPlane_(cameraAbsoluteLocation_, cameraLookAt_.unit())
            {
                ReferencedPointer<CameraComponentPrivate> cameraComponent = cameraNode_.component<CameraComponentPrivate>();
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = cameraNode_.component<TransformationComponentPrivate>();

                if(cameraComponent.isNull() || transformationComponent.isNull())
                {
                    throw Exception("Camera node doesn't contain camera or transformation component!");
                }
            }

            CullerBoxVisitor::~CullerBoxVisitor()
            {
            }

            void CullerBoxVisitor::resetCameraRelatedInformation() throw (Exception)
            {
                // fovy
                ReferencedPointer<CameraComponentPrivate> cameraComponent = cameraNode_.component<CameraComponentPrivate>();
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = cameraNode_.component<TransformationComponentPrivate>();

                if(cameraComponent.isNull() || transformationComponent.isNull())
                {
                    throw Exception("Camera node doesn't contain camera or transformation component!");
                }

                CameraComponentPrivate* cameraComponentPrivate = static_cast<CameraComponentPrivate*>(cameraComponent.pointer());
                TransformationComponentPrivate* transformationComponentPrivate = static_cast<TransformationComponentPrivate*>(transformationComponent.pointer());

                // location
                {
                    cameraAbsoluteLocation_ = transformationComponentPrivate->location();
                }

                // look at
                {
                    cameraLookAt_ = cameraComponentPrivate->lookAt(cameraAbsoluteLocation_, transformationComponentPrivate->rotation());
                }

                cameraPlane_ = Plane(cameraAbsoluteLocation_, cameraLookAt_.unit());
            }

            void CullerBoxVisitor::visitGraphEnter()
            {
                worldTransformationStack_.clear();
                worldTransformationStack_.push_back(Matrix4());
                worldOpacityStack_.clear();
                worldOpacityStack_.push_back(1.0f);
                resetCameraRelatedInformation();
            }

            void CullerBoxVisitor::visitGraphLeave()
            {
                // Do nothing
            }

            void CullerBoxVisitor::visitNodeEnter(NodePrivate* nodePrivate)
            {
                // Update active world transform
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = nodePrivate->component<TransformationComponentPrivate>();
                TransformationComponentPrivate* transformationComponentPrivate = transformationComponent.pointer();
                float largestScaleFactor = 1.0f;
                if(transformationComponentPrivate)
                {
                    worldTransformationStack_.push_back(transformationComponentPrivate->worldTransformation());
                    worldOpacityStack_.push_back(transformationComponentPrivate->worldOpacity());
                    largestScaleFactor = transformationComponentPrivate->largestWorldScaleFactor();
                }

                const Matrix4 &worldTransformation = worldTransformationStack_.back();
                const float worldOpacity = worldOpacityStack_.back();
                const float smallestCullingX = cullingBoxOrigo_.x() - cullingBoxAxis_.x();
                const float largesttCullingX = cullingBoxOrigo_.x() + cullingBoxAxis_.x();
                const float smallestCullingY = cullingBoxOrigo_.y() - cullingBoxAxis_.y();
                const float largesttCullingY = cullingBoxOrigo_.y() + cullingBoxAxis_.y();
                const float smallestCullingZ = cullingBoxOrigo_.z() - cullingBoxAxis_.z();
                const float largesttCullingZ = cullingBoxOrigo_.z() + cullingBoxAxis_.z();

                // Check if something to render (and so to cull)
                ReferencedPointer<ModelComponentPrivate> modelComponent = nodePrivate->component<ModelComponentPrivate>();

//#ifndef __NGOS__
//                ReferencedPointer<TextComponentPrivate> textComponent = nodePrivate->component<TextComponentPrivate>();
//                if (modelComponent.pointer() != NULL || textComponent.pointer() != NULL)
//#else
                if (modelComponent.pointer() != NULL)
//#endif
                {
                    // Object absolute location
                    Vector3 location(worldTransformation[0][3], worldTransformation[1][3], worldTransformation[2][3]);

                    // Vector from camera to object
                    Vector3 fromCameraToObject = location - cameraAbsoluteLocation_;
                    float distanceFromCamera = cameraPlane_.distanceToPoint(location);

                    // Closest point on camera lookAt ray
                    // Calculate angle
                    // s = dot(a, b) / unit(a)
                    //
                    // if s > 0, 0 < angle <= 90
                    // if s < 0, 0 < angle <= 180
                    float s = Vector3::dot(fromCameraToObject, cameraLookAt_.unit());
                    Vector3 closestPoint = cameraAbsoluteLocation_;
                    if (s >= 0)
                    {
                        closestPoint += cameraLookAt_.unit() * s;
                    }

                    if (modelComponent.pointer() != NULL)
                    {
                        ModelComponentPrivate* modelComponentPrivate = modelComponent.pointer();
                        std::vector<ModelComponent::MeshMaterialPair>& pairs = modelComponentPrivate->meshMaterialPairs();
                        for (std::vector<ModelComponent::MeshMaterialPair>::const_iterator i = pairs.begin(); i != pairs.end(); i++)
                        {
                            ModelComponent::MeshMaterialPair* pair = *(ModelComponent::MeshMaterialPair**)(&i); // WTF cast

                            float distanceFromClosestPointToObject = (location - closestPoint).length();
                            float fovConeOpening = s * fieldOfViewRatio_;
                            //MeshResource* meshResource = pair->meshAsset.resource();
                            float boundingR = pair->meshAsset.boundingSphereRadius() * largestScaleFactor;

                            if((location.x() + boundingR < smallestCullingX || location.x() - boundingR > largesttCullingX)
                                ||
                                (location.y() + boundingR < smallestCullingY || location.y() - boundingR > largesttCullingY)
                                ||
                                (location.z() + boundingR < smallestCullingZ || location.z() - boundingR > largesttCullingZ))
                            {
                                return;
                            }

                            // Create mesh-material pair to renderqueue
                            {
                                if (pair->materialAsset.resource()->colorMap().pointer() != NULL) // Gradient effect does not have colormap
                                {
                                    bool hasAlpha = pair->materialAsset.resource()->colorMap().pointer()->hasAlpha() || (worldOpacity < 1.0f);
                                    renderQueueObjects_.push_back(Renderer::RenderQueueObject(worldTransformationStack_.back(), distanceFromCamera, pair, hasAlpha, worldOpacity));
                                }
                                else if (pair->materialAsset.resource()->effect() == MaterialResource::Effect::Shadow)
                                {
                                    renderQueueObjects_.push_back(Renderer::RenderQueueObject(worldTransformationStack_.back(), distanceFromCamera, pair, true, worldOpacity));
                                }
                                else
                                {
                                    renderQueueObjects_.push_back(Renderer::RenderQueueObject(worldTransformationStack_.back(), distanceFromCamera, pair, false, worldOpacity));
                                }
                            }
                        }
                    }
#ifndef __NGOS__
                    /*if (textComponent.pointer() != NULL)
                    {
                        TextComponentPrivate* textComponentPrivate = textComponent.pointer();

                        float distanceFromClosestPointToObject = (location - closestPoint).length();
                        float fovConeOpening = s * fieldOfViewRatio_;
                        //float tmp = distanceFromClosestPointToObject - textComponentPrivate->boundingSphereRadius();
                        //if (tmp < fovConeOpening)
                        {
                            renderQueueObjects_.push_back(Renderer::RenderQueueObject(worldTransformationStack_.back(), distanceFromCamera, textComponentPrivate->kaskiText_, true, worldOpacity));
                        }
                    }*/
#endif
                }
            }

            void CullerBoxVisitor::visitNodeLeave(NodePrivate* nodePrivate)
            {
                // Update active world transform
                ReferencedPointer<AbstractComponentPrivate> transformationComponent = nodePrivate->component("TransformationComponent");
                TransformationComponentPrivate* transformationComponentPrivate = static_cast<TransformationComponentPrivate*>(transformationComponent.pointer());
                if(transformationComponentPrivate)
                {
                    worldTransformationStack_.pop_back();
                    worldOpacityStack_.pop_back();
                }
            }

            std::vector<Renderer::RenderQueueObject>& CullerBoxVisitor::renderQueueObjects()
            {
                return renderQueueObjects_;
            }

            void CullerBoxVisitor::setCullerBoxOrigo(Vector3 const& origo)
            {
                cullingBoxOrigo_ = origo;
            }

            void CullerBoxVisitor::setCullerBoxAxis(Vector3 const& axis)
            {
                cullingBoxAxis_ = axis;
            }
        }
    }
}
