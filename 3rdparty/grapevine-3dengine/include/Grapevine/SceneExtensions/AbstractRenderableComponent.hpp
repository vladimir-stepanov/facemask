/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ABSTRACTRENDERABLECOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_ABSTRACTRENDERABLECOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/SceneExtensions/AbstractRenderableComponentPrivate.hpp>
#include <string>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AbstractRenderableComponent: public Component
            {
            public:
                CLASS_COPYABLE(AbstractRenderableComponent)


            protected:
                AbstractRenderableComponent(AbstractRenderableComponentPrivate* private_);
                virtual ~AbstractRenderableComponent();

            private:
                class AbstractRenderableComponentPrivate* componentPrivate_;
            };
        }
    }
}

#endif
