/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include "Grapevine/Core/Timer.hpp"
#include <Grapevine/Config.hpp>

namespace Grapevine
{
    namespace Scene
    {
        AbstractTreeVisitor::AbstractTreeVisitor(std::string const& name):
            AbstractVisitor(name)
        {
        }

        AbstractTreeVisitor::~AbstractTreeVisitor()
        {
        }
    }
}
