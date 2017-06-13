/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ANIMATORVISITOR
#define GRAPEVINE_SCENEEXTENSIONS_ANIMATORVISITOR

#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include "Grapevine/Scene/NodePrivate.hpp"
#include "Grapevine/Core/Exception.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AnimatorVisitor: public AbstractTreeVisitor
            {
            public:
                AnimatorVisitor();
                ~AnimatorVisitor();

                // Inherited
                void visitGraphEnter();
                void visitGraphLeave();
                void visitNodeEnter(NodePrivate* nodePrivate);
                void visitNodeLeave(NodePrivate* nodePrivate);
                bool active() const;

            private:
                bool active_;
            };
        }
    }
}

#endif
