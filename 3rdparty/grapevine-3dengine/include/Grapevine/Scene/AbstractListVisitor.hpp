/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_ABSTRACTLISTVISITOR
#define GRAPEVINE_SCENE_ABSTRACTLISTVISITOR

#include "Grapevine/Scene/AbstractVisitor.hpp"
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/Class.hpp>
#include <string>
#include <Grapevine/Config.hpp>
#ifdef GRAPEVINE_CONFIG_DEBUG
    #include <map>
#endif

namespace Grapevine
{
    namespace Scene
    {
        class NodePrivate;

        /**
         * @class AbstractListVisitor
         *
         * AbstractListVisitor
         */
        class AbstractListVisitor: public AbstractVisitor
        {
        public:
            CLASS_UNCOPYABLE(AbstractListVisitor)

            /**
             * @param [in]  name    used for debugging purposes. Please use your class name for this.
             */
            AbstractListVisitor(std::string const& name);
            virtual ~AbstractListVisitor();
        };
    }
}

#endif
