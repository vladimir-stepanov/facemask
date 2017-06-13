/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/InfoComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            InfoComponentPrivate::InfoComponentPrivate(std::string const& name):
                AbstractComponentPrivateTemplate<InfoComponentPrivate>("InfoComponent"),
                name_(name)
            {
            }

            InfoComponentPrivate::~InfoComponentPrivate()
            {
            }

            void InfoComponentPrivate::setName(std::string const& name)
            {
                name_ = name;
            }

            std::string const& InfoComponentPrivate::name()
            {
                return name_;
            }
        }
    }
}
