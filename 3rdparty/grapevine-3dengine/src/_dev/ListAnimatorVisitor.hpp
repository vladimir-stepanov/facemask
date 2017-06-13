/**
 * Copyright (C) 2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_LISTANIMATORVISITOR
#define GRAPEVINE_SCENEEXTENSIONS_LISTANIMATORVISITOR

#include "Grapevine/Scene/AbstractListVisitor.hpp"
#include "Grapevine/Scene/NodePrivate.hpp"
#include "Grapevine/Core/Exception.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class ListAnimatorVisitor: public Scene::AbstractListVisitor
            {
            public:
                ListAnimatorVisitor();
                virtual ~ListAnimatorVisitor(){};
                virtual void visitNodeEnter(Scene::NodePrivate* node);
                virtual void visitNodeLeave(Scene::NodePrivate* node);
            };
        }
    }
}

#endif
