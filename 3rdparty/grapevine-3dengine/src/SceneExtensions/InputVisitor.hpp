/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_EXTENSIONS_INPUTVISITOR
#define GRAPEVINE_SCENE_EXTENSIONS_INPUTVISITOR

#include "Grapevine/SceneExtensions/InputComponent.hpp"
#include "Grapevine/Scene/AbstractListVisitor.hpp"
#include <Grapevine/Scene/NodePrivate.hpp>
#include <Grapevine/Input/InputEvent.hpp>
#include <Grapevine/Math/Ray.hpp>
#include <vector>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class InputVisitor: public Scene::AbstractListVisitor
            {
            public:
                InputVisitor(std::vector<InputComponent::Event> const& inputEvents);
                virtual ~InputVisitor();
                virtual void visitNodeEnter(Scene::NodePrivate* node);
                virtual void visitNodeLeave(Scene::NodePrivate* node);

            private:
                std::vector<InputComponent::Event> inputEvents_;
            };
        }
    }
}

#endif
