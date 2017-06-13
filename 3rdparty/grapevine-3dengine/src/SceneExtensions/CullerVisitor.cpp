/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "CullerVisitor.hpp"
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
            CullerVisitor::CullerVisitor(Node cameraNode):
                AbstractTreeVisitor("CullerVisitor"),
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

            CullerVisitor::~CullerVisitor()
            {
            }

            void CullerVisitor::resetCameraRelatedInformation() throw (Exception)
            {
                ReferencedPointer<CameraComponentPrivate> cameraComponent = cameraNode_.component<CameraComponentPrivate>(); //("CameraComponent");
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = cameraNode_.component<TransformationComponentPrivate>(); //("TransformationComponent");
                if(cameraComponent.isNull() || transformationComponent.isNull())
                {
                    throw Exception("Camera node doesn't contain camera or transformation component!");
                }

                // fovy
                CameraComponentPrivate* cameraComponentPrivate = static_cast<CameraComponentPrivate*>(cameraComponent.pointer());
                TransformationComponentPrivate* transformationComponentPrivate = static_cast<TransformationComponentPrivate*>(transformationComponent.pointer());

                // fovy
                {
                    float aspectRatio = cameraComponentPrivate->aspectRatio();

                    // Calculate "tube" frustum fieldOfView
                    //
                    // We calculate the ratio how much the frustum opens per
                    // one unit. This way we can see if the objects are in the
                    // frustum cone.

                    // 1 height unit is the camera's fovy
                    float fovy = cameraComponentPrivate->fieldOfViewY();

                    // 0.5 height unit is 0.5 fovy
                    float fovy05 = 0.5f * fovy;

                    // Distance D where 0.5 height unit is "achieved"
                    float D = 0.5f / tanf((fovy05 * 3.1415f / 180.0f));
                    float ratio0 = 0.5f / D;

                    // Calculate height H unit for tube frustum
                    float H = sqrt((aspectRatio / 2.0f) * (aspectRatio / 2.0f) + (1.0f / 2.0f) * (1.0f / 2.0f));

                    // For each unit we go in camera lookAt, the tube frustum opens
                    // fieldOfViewRatio_ amount
                    fieldOfViewRatio_ = (H / 0.5f) * ratio0;
                }

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

            void CullerVisitor::visitGraphEnter()
            {
                worldTransformationStack_.clear();
                worldTransformationStack_.push_back(Matrix4());
                worldOpacityStack_.clear();
                worldOpacityStack_.push_back(1.0f);
                resetCameraRelatedInformation();
            }

            void CullerVisitor::visitGraphLeave()
            {
                // Do nothing
            }

            void CullerVisitor::visitNodeEnter(NodePrivate* nodePrivate)
            {
                // Update active world transform
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = nodePrivate->component<TransformationComponentPrivate>();
                TransformationComponentPrivate* transformationComponentPrivate = transformationComponent.pointer();
                if(transformationComponentPrivate)
                {
                    worldTransformationStack_.push_back(transformationComponentPrivate->worldTransformation());
                    worldOpacityStack_.push_back(transformationComponentPrivate->worldOpacity());
                }

                const Matrix4 &worldTransformation = worldTransformationStack_.back();
                const float worldOpacity = worldOpacityStack_.back();

                if(!worldOpacity)
                {
                    return;
                }

                // Check if something to render (and so to cull)
                ReferencedPointer<ModelComponentPrivate> modelComponent = nodePrivate->component<ModelComponentPrivate>(); //("ModelComponentPrivate");

//#ifndef __NGOS__
//                ReferencedPointer<TextComponentPrivate> textComponent = nodePrivate->component<TextComponentPrivate>(); //("TextComponentPrivate");
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
                            float tmp = distanceFromClosestPointToObject - pair->meshAsset.boundingSphereRadius();
                            if (tmp < fovConeOpening)
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
#ifndef __NGOS__
                                    renderQueueObjects_.push_back(Renderer::RenderQueueObject(worldTransformationStack_.back(), distanceFromCamera, pair, false, worldOpacity));
#endif
                                }
                            }
                        }
                    }
/*
#ifndef __NGOS__

                    if (textComponent.pointer() != NULL)
                    {
                        TextComponentPrivate* textComponentPrivate = textComponent.pointer();

                        float distanceFromClosestPointToObject = (location - closestPoint).length();
                        float fovConeOpening = s * fieldOfViewRatio_;
                        float tmp = distanceFromClosestPointToObject - textComponentPrivate->boundingSphereRadius();
                        if (tmp < fovConeOpening)
                        {
                            renderQueueObjects_.push_back(Renderer::RenderQueueObject(worldTransformationStack_.back(), distanceFromCamera, NULL, true, worldOpacity));
                        }
                    }
#endif
*/
                }
            }

            void CullerVisitor::visitNodeLeave(NodePrivate* nodePrivate)
            {
                // Update active world transform
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = nodePrivate->component<TransformationComponentPrivate>();
                TransformationComponentPrivate* transformationComponentPrivate = transformationComponent.pointer();
                if(transformationComponentPrivate)
                {
                    worldTransformationStack_.pop_back();
                    worldOpacityStack_.pop_back();
                }
            }

            bool CullerVisitor::canVisit(NodePrivate* nodePrivate)
            {
                return worldOpacityStack_.back() > 0.f;
            }

            std::vector<Renderer::RenderQueueObject>& CullerVisitor::renderQueueObjects()
            {
                return renderQueueObjects_;
            }
        }
    }
}
