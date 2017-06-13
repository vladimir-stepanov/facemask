/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

// Set me to false to disable "idle draw removal" feature
static const bool USE_IDLE_DRAW_REMOVAL = true;

#include "Grapevine/SceneExtensions/SimpleScene.hpp"
#include "TransformVisitor.hpp"
//#include "CullerBoxVisitor.hpp"
#include "CullerVisitor.hpp"
#include "InputVisitor.hpp"
//#include "ListAnimatorVisitor.hpp"
#include "Grapevine/Scene/AbstractComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/Renderer.hpp"
//#include <../src/kaski/text.hpp>
#include "Grapevine/Core/Timer.hpp"
//#include "Grapevine/Animation/AnimationManager.hpp"
//#include "Grapevine/SceneExtensions/AnimationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
//#include "Grapevine/SceneExtensions/TextComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/CameraComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/InputComponentPrivate.hpp"
#include <Grapevine/Config.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            // Comparison function
            bool compareAgainstCameraLocation(Renderer::RenderQueueObject& obj0, Renderer::RenderQueueObject& obj1)
            {
                if (obj0.distanceFromCamera > obj1.distanceFromCamera)
                {
                    return true;
                }
                return false;
            }

            SimpleScene::SimpleScene(Node cameraNode):
                cullingMethod_(CullingMethod::CameraFrustum),
                cullingBoxOrigo_(Vector3()),
                cullingBoxAxis_(Vector3()),
                fpsOverRenderQueueSort_("FPS(renderqueue sort)"),
                fpsOverDraw_("FPS(only draw commands)"),
                lastOrientationTimestamp_(0),
                lastOrientationTimestampDiff_(0),
                renderedOrientation_(Vector::origo),
                currentOrientation_(Vector::origo),
                lastOrientation_(Vector::origo),
                cameraNode_(cameraNode),
                renderer_(new Renderer(cameraNode)),
                sceneDirtyDebugFlag_(false)
            {
            }

            SimpleScene::SimpleScene(Node& cameraNode, Vector3 const& cullerBoxOrigo, Vector3 const& cullerBoxHalfAxis):
                cullingMethod_(CullingMethod::Box),
                cullingBoxOrigo_(cullerBoxOrigo),
                cullingBoxAxis_(cullerBoxHalfAxis),
                fpsOverRenderQueueSort_("FPS(renderqueue sort)"),
                fpsOverDraw_("FPS(only draw commands)"),
                lastOrientationTimestamp_(0),
                lastOrientationTimestampDiff_(0),
                renderedOrientation_(Vector::origo),
                currentOrientation_(Vector::origo),
                lastOrientation_(Vector::origo),
                cameraNode_(cameraNode),
                renderer_(new Renderer(cameraNode)),
                sceneDirtyDebugFlag_(false)
            {
            }

            SimpleScene::~SimpleScene()
            {
                if (renderer_ != NULL)
                {
                    delete renderer_;
                }
            }

            bool SimpleScene::draw(Canvas& canvas)
            {
                static unsigned int prevDrawTime = 0;
                static unsigned int drawTime = 0;

                prevDrawTime = drawTime;
                drawTime = Timer::milliseconds();
                float frameTimeStep = float(drawTime - prevDrawTime);

                //AnimationManager::setFrameTimestampMilliseconds(drawTime);

                // Device orientation
                if (lastOrientationTimestampDiff_ != 0)
                {
                    Vector3 tmp = currentOrientation_ - lastOrientation_;
                    tmp /= (float)lastOrientationTimestampDiff_;
                    renderedOrientation_ += (tmp * frameTimeStep);
                    // TODO: check this one, probably not right now
                    Matrix4 orientationX = Matrix::rotate(renderedOrientation_.x(), Vector::xAxis);
                    Matrix4 orientationY = Matrix::rotate(renderedOrientation_.y(), Vector::yAxis);
                    renderer_->setDeviceOrientation(orientationX * orientationY);
                }

                // Animation
                //ListAnimatorVisitor animationVisitor;
                //Scene::NodeList animNodes = Scene::Node::componentOwners<AnimationComponentPrivate>();
                //animNodes.accept(animationVisitor);

                // Transform
                TransformVisitor transformVisitor;
                accept(transformVisitor);

                // Input
                InputVisitor inputVisitor(inputEvents_);
                Scene::NodeList inputNodes = Scene::Node::componentOwners<InputComponentPrivate>();
                inputNodes.accept(inputVisitor);
                inputEvents_.clear();

                // Cull & sort
#ifdef GRAPEVINE_CONFIG_DEBUG
                static float cullingLastDebugTime = Timer::milliseconds();
                static unsigned int nodesRenderedAlphas = 0;
                static unsigned int nodesRenderedNonAlphas = 0;
                static unsigned int framesCulledAgainstCullerDebugPrint = 0;
                fpsOverRenderQueueSort_.startStep();
#endif
                std::list<Renderer::RenderQueueObject> alphas;
                std::list<Renderer::RenderQueueObject> nonAlphas;
                {
                    LOGD("culling..");
                    if(cullingMethod_ == CullingMethod::CameraFrustum)
                    {
                    LOGD("culling.2.1");
                        CullerVisitor cullerVisitor(cameraNode_);
                    LOGD("culling.2.2");
                        accept(cullerVisitor);
                    LOGD("culling.2.3");
                        std::vector<Renderer::RenderQueueObject>& objects = cullerVisitor.renderQueueObjects();
                        while (objects.size() > 0)
                        {
                            Renderer::RenderQueueObject tmp = objects.back();
                            if (tmp.hasAlpha == true)
                            {
                                alphas.push_back(tmp);
                            }
                            else
                            {
                                nonAlphas.push_back(tmp);
                            }
                            objects.pop_back();
                        }
                    }
                    else if(cullingMethod_ == CullingMethod::Box)
                    {
                        LOGE("Box culling not supported currently");
                        /*
                        CullerBoxVisitor cullerVisitor(cameraNode_, cullingBoxOrigo_, cullingBoxAxis_);
                        accept(cullerVisitor);

                        std::vector<Renderer::RenderQueueObject>& objects = cullerVisitor.renderQueueObjects();
                        while (objects.size() > 0)
                        {
                            Renderer::RenderQueueObject tmp = objects.back();
                            if (tmp.hasAlpha == true)
                            {
                                alphas.push_back(tmp);
                            }
                            else
                            {
                                nonAlphas.push_back(tmp);
                            }
                            objects.pop_back();
                        }*/
                    }
#ifdef GRAPEVINE_CONFIG_DEBUG
                    nodesRenderedAlphas += alphas.size();
                    nodesRenderedNonAlphas += nonAlphas.size();
                    framesCulledAgainstCullerDebugPrint++;
#endif
                }
#ifdef GRAPEVINE_CONFIG_DEBUG
                if(drawTime > cullingLastDebugTime + 4000)
                {
                    float avAlphas = (float)nodesRenderedAlphas / (float)framesCulledAgainstCullerDebugPrint;
                    float avNonAlphas = (float)nodesRenderedNonAlphas / (float)framesCulledAgainstCullerDebugPrint;
                    cullingLastDebugTime = drawTime;
                    nodesRenderedAlphas = 0;
                    nodesRenderedNonAlphas = 0;
                    framesCulledAgainstCullerDebugPrint = 0;
                    LOGD("Nodes rendered per frame av. %f, (alphas: %f, non-alphas: %f)", avAlphas + avNonAlphas, avAlphas, avNonAlphas);
                    LOGD("Total node amount %d", NodePrivate::nodeAmount());
                    LOGD("Total component amount %d", AbstractComponentPrivate::componentAmount());
                    //LOGD("Total component amount %d", ComponentPrivate::totalAmount());
                }
                fpsOverRenderQueueSort_.endStep();
                fpsOverDraw_.startStep();
#endif

                // Init renderer
                {
                    renderer_->initFrame();
                    {
                        canvas.clear();

                        // Draw nonAlphas
                        for (std::list<Renderer::RenderQueueObject>::iterator i = nonAlphas.begin(); i != nonAlphas.end(); i++)
                        {
                            Renderer::RenderQueueObject tmp = *i;
                            if (tmp.meshMaterialPair != NULL)
                            {
                                renderer_->render(tmp.transformation, tmp.meshMaterialPair->materialAsset.resource(), tmp.meshMaterialPair->meshAsset.resource());
                            }
/*
#ifndef __NGOS__
                            if (tmp.text != NULL)
                            {
                                renderer_->render(tmp.transformation, tmp.text, 0, tmp.text->getGlyphCount());
                            }
#endif
* */
                        }

                        // Draw alphas
                        renderer_->disableDepthWrites();
                        alphas.sort(compareAgainstCameraLocation);
                        for (std::list<Renderer::RenderQueueObject>::iterator i = alphas.begin(); i != alphas.end(); i++)
                        {
                            Renderer::RenderQueueObject tmp = *i;
                            if (tmp.meshMaterialPair != NULL)
                            {
                                renderer_->render(tmp.transformation, tmp.meshMaterialPair->materialAsset.resource(), tmp.meshMaterialPair->meshAsset.resource(), tmp.opacity);
                            }
/*

#ifndef __NGOS__

                            if (tmp.text != NULL)
                            {
                                renderer_->render(tmp.transformation, tmp.text, 0, tmp.text->getGlyphCount(), tmp.opacity);
                            }
#endif
* */
                        }
                        renderer_->enableDepthWrites();

                        canvas.update();
                    }
                }

#ifdef GRAPEVINE_CONFIG_DEBUG
                fpsOverDraw_.endStep();
                AbstractVisitor::debugStep();
#endif

                if (AnimationManager::sceneDirty() == false && USE_IDLE_DRAW_REMOVAL)
                {
                    if(sceneDirtyDebugFlag_ == true)
                    {
                        LOGD("dirty -> not dirty");
                    }
                    sceneDirtyDebugFlag_ = false;
                    return false;
                }

                if(sceneDirtyDebugFlag_ == false)
                {
                    LOGD("not dirty -> dirty");
                }
                sceneDirtyDebugFlag_ = true;

                AnimationManager::setSceneDirty(false);
                return true;
            }

            void SimpleScene::setLightDirection(Vector3 const& direction)
            {
                renderer_->setLightDiffuseDirection(direction.normal());
                renderer_->setLightSpecularDirection(direction.normal());
            }

            void SimpleScene::setOmniLightWorldLocation(Vector3 const& worldLocation)
            {
                renderer_->setOmniLightWorldLocation(worldLocation);
            }

            void SimpleScene::setOmniLightColor(Vector3 const& color)
            {
                renderer_->setOmniLightColor(color);
            }

            void SimpleScene::setOmniLightStrength(float strength)
            {
                renderer_->setOmniLightStrength(strength);
            }

            void SimpleScene::setDeviceOrientation(Vector3 const& currentOrientation)
            {
                unsigned int tmp = lastOrientationTimestamp_;
                lastOrientationTimestamp_ = Timer::milliseconds();
                if (tmp != 0)
                {
                    lastOrientationTimestampDiff_ = lastOrientationTimestamp_ - tmp;
                }

                lastOrientation_ = currentOrientation_;
                currentOrientation_ = currentOrientation;
            }

            void SimpleScene::setDirty()
            {
                AnimationManager::setSceneDirty(true);
            }

            void SimpleScene::setCullerBoxOrigo(Vector3 const& origo)
            {
                cullingBoxOrigo_ = origo;
            }

            void SimpleScene::setCullerBoxAxis(Vector3 const& axis)
            {
                cullingBoxAxis_ = axis;
            }

            void SimpleScene::add(InputComponent::Event const& event)
            {
                inputEvents_.push_back(event);
            }
        }
    }
}
