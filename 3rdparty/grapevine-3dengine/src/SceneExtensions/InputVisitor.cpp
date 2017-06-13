/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "InputVisitor.hpp"
#include "Grapevine/SceneExtensions/InputComponent.hpp"
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/InputComponentPrivate.hpp"
#include <Grapevine/Math/Sphere.hpp>
#include <Grapevine/Math/Ray.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            InputVisitor::InputVisitor(std::vector<InputComponent::Event> const& inputEvents):
                AbstractListVisitor("InputVisitor"),
                inputEvents_(inputEvents)
            {
            }

            InputVisitor::~InputVisitor()
            {
            }

            void InputVisitor::visitNodeEnter(NodePrivate* nodePrivate)
            {
                ReferencedPointer<TransformationComponentPrivate> transformationComponent = nodePrivate->component<TransformationComponentPrivate>();
                ReferencedPointer<ModelComponentPrivate> modelComponent = nodePrivate->component<ModelComponentPrivate>();

                if(modelComponent.exists() && transformationComponent.exists())
                {
                    ReferencedPointer<InputComponentPrivate> inputComponent = nodePrivate->component<InputComponentPrivate>();
                    Matrix4 worldTransformation = transformationComponent.pointer()->worldTransformation();

                    // Look all meshes through from model
                    std::vector<ModelComponent::MeshMaterialPair>& meshMaterialPairs = modelComponent.pointer()->meshMaterialPairs();
                    for(std::vector<ModelComponent::MeshMaterialPair>::iterator i = meshMaterialPairs.begin();
                        i != meshMaterialPairs.end();
                        i++)
                    {
                        // Look all input events through
                        float boundingSphereRadius = (*i).meshAsset.boundingSphereRadius();
                        Sphere boundingSphere(Vector3(0, 0, 0), boundingSphereRadius);
                        for(std::vector<InputComponent::Event>::iterator iInput = inputEvents_.begin(); iInput != inputEvents_.end(); iInput++)
                        {
                            Ray rayObjectSpace = Ray::transform((*iInput).ray, worldTransformation);
                            RaySphereIntersection intersection = rayObjectSpace.intersection(boundingSphere);
                            if(intersection.type() != RaySphereIntersection::Type::NoIntersection)
                            {
                                inputComponent.pointer()->input.emit(*iInput, inputComponent.pointer()->userData());
                            }
                        }
                    }
                }
            }

            void InputVisitor::visitNodeLeave(NodePrivate* nodePrivate)
            {
            }
        }
    }
}
