/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_EXTENSIONS_CULLERVISITOR
#define GRAPEVINE_SCENE_EXTENSIONS_CULLERVISITOR

#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include "Grapevine/Scene/Node.hpp"
#include "Grapevine/Math/Matrix.hpp"
#include "Grapevine/Math/Ray.hpp"
#include "Grapevine/SceneExtensions/ModelComponent.hpp"
#include "Grapevine/Core/Exception.hpp"
#include "Grapevine/Math/Plane.hpp"
#include "Grapevine/SceneExtensions/Renderer.hpp"
#include <vector>
#include <stdexcept>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class SceneObject;

            class CullerVisitor: public AbstractTreeVisitor
            {
            public:
                CullerVisitor(Node cameraNode);
                ~CullerVisitor();

                // Inherited
                void visitGraphEnter();
                void visitGraphLeave();
                void visitNodeEnter(NodePrivate* nodePrivate);
                void visitNodeLeave(NodePrivate* nodePrivate);

                bool canVisit(NodePrivate* nodePrivate);

                // Own
                void checkVisibility(SceneObject* object);
                std::vector<Renderer::RenderQueueObject>& renderQueueObjects();

            private:
                void resetCameraRelatedInformation() throw (Exception);
                std::vector<Matrix4> worldTransformationStack_;
                std::vector<float> worldOpacityStack_;
                Node cameraNode_;

                Plane frustumSidePlanes_[4];

                // Updated in visitGraphEnter()
                float fieldOfViewRatio_;
                Vector3 cameraAbsoluteLocation_;
                Ray cameraLookAt_;
                Plane cameraPlane_;

                // What was visible
                std::vector<Renderer::RenderQueueObject> renderQueueObjects_;

            };
        }
    }
}

#endif
