/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/InfoComponent.hpp"
#include "Grapevine/SceneExtensions/InfoComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            InfoComponent::InfoComponent(std::string const& name):
                Component(new InfoComponentPrivate(name))
            {
                infoComponentPrivate_ = dynamic_cast<InfoComponentPrivate*>(private_.pointer());
            }

            InfoComponent::InfoComponent(InfoComponent const &r)
            : Component(r)
            {
                infoComponentPrivate_ = r.infoComponentPrivate_;
            }

            InfoComponent::~InfoComponent()
            {
            }

            InfoComponent &InfoComponent::operator=(InfoComponent const &r)
            {
                infoComponentPrivate_ = r.infoComponentPrivate_;
                Component::operator=(r);
                return *this;
            }

            void InfoComponent::setName(std::string const& name)
            {
                infoComponentPrivate_->setName(name);
            }

            std::string const& InfoComponent::name()
            {
                return infoComponentPrivate_->name();
            }
        }
    }
}
