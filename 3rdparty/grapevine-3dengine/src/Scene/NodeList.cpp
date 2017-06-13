/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/NodeList.hpp"
#include "Grapevine/Scene/NodeListPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        NodeList::NodeList(std::vector<ReferencedPointer<NodePrivate> > const& list):
            ReferencedPointer<NodeListPrivate>(new NodeListPrivate(list))
        {
        }

        NodeList::~NodeList()
        {
        }

        void NodeList::accept(AbstractListVisitor& visitor)
        {
            pointer()->accept(visitor);
        }

        NodeList NodeList::booleanAnd(NodeList const& a, NodeList const& b)
        {
            return NodeList(NodeListPrivate::booleanAnd(a.pointer(), b.pointer()));
        }

        NodeList NodeList::booleanOr(NodeList const& a, NodeList const& b)
        {
            return NodeList(NodeListPrivate::booleanOr(a.pointer(), b.pointer()));
        }

        unsigned int NodeList::length() const
        {
            return pointer()->length();
        }
    }
}
