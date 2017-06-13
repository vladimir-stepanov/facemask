/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "TransformVisitor.hpp"
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include <Grapevine/Scene/NodePrivate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            TransformVisitor::TransformVisitor():
                AbstractTreeVisitor("TransformVisitor")
            {
            }

            TransformVisitor::~TransformVisitor()
            {
            }

            void TransformVisitor::visitGraphEnter()
            {
                matrixStack_.clear();
            }

            void TransformVisitor::visitGraphLeave()
            {
                // Do nothing
            }

            void TransformVisitor::visitNodeEnter(NodePrivate* nodePrivate)
            {
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = nodePrivate->component<TransformationComponentPrivate>();
                if (transformationComponent.pointer() != NULL)
                {
                    // Update matrix stack
                    TransformationComponentPrivate* transformationComponentPrivate = transformationComponent.pointer();
                    Matrix4 currentTransformation = transformationComponentPrivate->transformation();
                    float currentOpacity = transformationComponentPrivate->opacity();
                    Vector3 currentScaling = transformationComponentPrivate->scale();
                    if (matrixStack_.size() > 0)
                    {
                        currentTransformation = matrixStack_.back() * currentTransformation;
                        currentOpacity = opacityStack_.back() * currentOpacity;
                        currentScaling = scalingStack_.back() * currentScaling;
                    }
                    matrixStack_.push_back(currentTransformation);
                    opacityStack_.push_back(currentOpacity);
                    scalingStack_.push_back(currentScaling);

                    transformationComponentPrivate->setWorldTransformation(currentTransformation);
                    transformationComponentPrivate->setWorldOpacity(currentOpacity);
                    transformationComponentPrivate->setWorldScaling(currentScaling);
                }
                else
                {
                    // Did not have transformation node, use identity matrix
                    if (matrixStack_.size() > 0)
                    {
                        matrixStack_.push_back(matrixStack_.back());
                        opacityStack_.push_back(opacityStack_.back());
                        scalingStack_.push_back(scalingStack_.back());
                    }
                    else
                    {
                        matrixStack_.push_back(Matrix4());
                        opacityStack_.push_back(1.0f);
                        scalingStack_.push_back(Vector3(1.0f, 1.0f, 1.0f));
                    }
                }
            }

            void TransformVisitor::visitNodeLeave(NodePrivate* nodePrivate)
            {
                matrixStack_.pop_back();
                opacityStack_.pop_back();
                scalingStack_.pop_back();
            }
        }
    }
}
