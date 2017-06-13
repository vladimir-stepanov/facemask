/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_NODELIST
#define GRAPEVINE_SCENE_NODELIST

#include "Grapevine/Scene/NodePrivate.hpp"
#include "Grapevine/Scene/AbstractListVisitor.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class NodeList
         */
        class NodeList: public ReferencedPointer<class NodeListPrivate>
        {
        public:
            CLASS_COPYABLE(NodeList)

            /**
             * Accept visitor.
             *
             * @param [in]  visitor     Visitor visiting the node list's nodes.
             */
            void accept(AbstractListVisitor& visitor);
            NodeList(std::vector<ReferencedPointer<NodePrivate> > const& list);
            virtual ~NodeList();
            static NodeList booleanAnd(NodeList const& a, NodeList const& b);
            static NodeList booleanOr(NodeList const& a, NodeList const& b);
            unsigned int length() const;
        };
    }
}

#endif
