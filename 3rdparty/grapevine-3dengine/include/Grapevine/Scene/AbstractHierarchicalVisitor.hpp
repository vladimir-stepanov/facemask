/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_ABSTRACTHIERARCHICALVISITOR
#define GRAPEVINE_SCENE_ABSTRACTHIERARCHICALVISITOR

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
         * @class AbstractHierarchicalVisitor
         *
         * AbstractHierarchicalVisitor
         */
        class AbstractGraphVisitor: public AbstractVisitor
        {
        public:
            CLASS_UNCOPYABLE(AbstractHierarchicalVisitor)

            /**
             * @param [in]  name    used for debugging purposes. Please use your class name for this.
             */
            AbstractHierarchicalVisitor(std::string const& name);
            virtual ~AbstractHierarchicalVisitor();

            /**
             * Called when visitor is entering the graph.
             */
            virtual void visitGraphEnter() = 0;

            /**
             * Called when visitor is leaving the graph.
             */
            virtual void visitGraphLeave() = 0;
        };
    }
}

#endif
