/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/AbstractRenderableComponent.hpp"
#include "Grapevine/SceneExtensions/AbstractRenderableComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AbstractRenderableComponent::AbstractRenderableComponent(AbstractRenderableComponentPrivate* private_):
                Component(private_)
            {
                componentPrivate_ = private_;
            }

            AbstractRenderableComponent::AbstractRenderableComponent(AbstractRenderableComponent const &r):
                Component(r)
            {
                componentPrivate_ = r.componentPrivate_;
            }

            AbstractRenderableComponent::~AbstractRenderableComponent()
            {
            }

            AbstractRenderableComponent &AbstractRenderableComponent::operator=(AbstractRenderableComponent const &r)
            {
                Component::operator=(r);
                componentPrivate_ = r.componentPrivate_;
                return *this;
            }
        }
    }
}
