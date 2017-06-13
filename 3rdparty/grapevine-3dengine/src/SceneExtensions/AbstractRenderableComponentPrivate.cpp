/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/AbstractRenderableComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AbstractRenderableComponentPrivate::AbstractRenderableComponentPrivate(std::string const& type):
                AbstractComponentPrivateTemplate<AbstractRenderableComponentPrivate>(type)
            {
            }

            AbstractRenderableComponentPrivate::~AbstractRenderableComponentPrivate()
            {
            }
        }
    }
}
