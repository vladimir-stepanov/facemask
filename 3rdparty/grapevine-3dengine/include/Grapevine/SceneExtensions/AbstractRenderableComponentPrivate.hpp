/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ABSTRACTRENDERABLECOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_ABSTRACTRENDERABLECOMPONENTPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>
#include <string>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AbstractRenderableComponentPrivate: public AbstractComponentPrivateTemplate<AbstractRenderableComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(AbstractRenderableComponentPrivate)

            protected:
                AbstractRenderableComponentPrivate(std::string const& type);
                virtual ~AbstractRenderableComponentPrivate();
            };
        }
    }
}

#endif
