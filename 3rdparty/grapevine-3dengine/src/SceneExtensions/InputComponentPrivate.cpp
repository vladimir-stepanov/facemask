/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/SceneExtensions/InputComponentPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            InputComponentPrivate::InputComponentPrivate(ReferencedPointer<Referenced> const& userData):
                AbstractComponentPrivateTemplate<InputComponentPrivate>("InputComponentPrivate"),
                userData_(userData)
            {
            }

            InputComponentPrivate::~InputComponentPrivate()
            {
            }

            ReferencedPointer<Referenced> const& InputComponentPrivate::userData()
            {
                return userData_;
            }
        }
    }
}
