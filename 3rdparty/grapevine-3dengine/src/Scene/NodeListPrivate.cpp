/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/NodeListPrivate.hpp"
#include <set>

namespace Grapevine
{
    namespace Scene
    {
        NodeListPrivate::NodeListPrivate(std::vector<ReferencedPointer<NodePrivate> > const& list):
            list_(list)
        {
        }

        NodeListPrivate::~NodeListPrivate()
        {
        }

        void NodeListPrivate::accept(AbstractListVisitor& visitor)
        {
            for(std::vector<ReferencedPointer<NodePrivate> >::iterator i = list_.begin(); i != list_.end(); i++)
            {
                (*i).pointer()->accept(visitor);
            }
        }

        std::vector<ReferencedPointer<NodePrivate> > NodeListPrivate::booleanAnd(NodeListPrivate* a, NodeListPrivate* b)
        {
            std::vector<ReferencedPointer<NodePrivate> > r;
            for(std::vector<ReferencedPointer<NodePrivate> >::const_iterator iA = a->list_.begin(); iA != a->list_.end(); iA++)
            {
                for(std::vector<ReferencedPointer<NodePrivate> >::const_iterator iB = b->list_.begin(); iB != b->list_.end(); iB++)
                {
                    if((*iA).pointer()->id() == (*iB).pointer()->id())
                    {
                        r.push_back(ReferencedPointer<NodePrivate>(*iA));
                    }
                }
            }
            return r;
        }

        std::vector<ReferencedPointer<NodePrivate> > NodeListPrivate::booleanOr(NodeListPrivate* a, NodeListPrivate* b)
        {
            std::vector<ReferencedPointer<NodePrivate> > r;
            std::set<unsigned int> tmp;

            for(std::vector<ReferencedPointer<NodePrivate> >::const_iterator i = a->list_.begin(); i != a->list_.end(); i++)
            {
                r.push_back(ReferencedPointer<NodePrivate>(*i));
                tmp.insert((*i).pointer()->id());
            }

            for(std::vector<ReferencedPointer<NodePrivate> >::const_iterator i = b->list_.begin(); i != b->list_.end(); i++)
            {
                if(tmp.find((*i).pointer()->id()) == tmp.end())
                {
                    r.push_back(ReferencedPointer<NodePrivate>(*i));
                }
            }

            return r;
        }

        unsigned int NodeListPrivate::length() const
        {
            return list_.size();
        }
    }
}
