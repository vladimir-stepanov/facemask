/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_EXTENSIONS_TRANSFORMVISITOR
#define GRAPEVINE_SCENE_EXTENSIONS_TRANSFORMVISITOR

#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include "Grapevine/Scene/Node.hpp"
#include "Grapevine/Math/Matrix.hpp"
#include <vector>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class TransformVisitor: public AbstractTreeVisitor
            {
            public:
                TransformVisitor();
                ~TransformVisitor();

                // Inherited
                void visitGraphEnter();
                void visitGraphLeave();
                void visitNodeEnter(NodePrivate* nodePrivate);
                void visitNodeLeave(NodePrivate* nodePrivate);

            private:
                std::vector<Matrix4> matrixStack_;
                std::vector<Vector3> scalingStack_;
                std::vector<float> opacityStack_;
            };
        }
    }
}

#endif
