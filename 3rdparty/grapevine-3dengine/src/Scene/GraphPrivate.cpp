/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/Graph.hpp"
#include "GraphPrivate.hpp"
#include "Grapevine/Scene/NodePrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        GraphPrivate::GraphPrivate()
        {
        }

        GraphPrivate::~GraphPrivate()
        {
            for(std::list<NodePrivate*>::iterator i = rootNodes_.begin(); i != rootNodes_.end(); i++)
            {
                NodePrivate* tmp = *i;
                tmp->decrement();
                tmp = NULL;
            }

            NodePrivate::releaseNodesWithComponentList();
        }

        void GraphPrivate::accept(AbstractTreeVisitor& visitor)
        {
            visitor.visitGraphEnter();

            for (std::list<NodePrivate*>::iterator i = rootNodes_.begin(); i != rootNodes_.end(); i++)
            {
                (*i)->accept(visitor);
            }

            visitor.visitGraphLeave();
        }

        void GraphPrivate::addNode(NodePrivate* node)
        {
            rootNodes_.push_back(node);
            node->increment();
        }

        void GraphPrivate::remove(NodePrivate* node)
        {
            for (std::list<NodePrivate*>::iterator i = rootNodes_.begin(); i != rootNodes_.end(); i++)
            {
                if (*i == node)
                {
                    LOG
                    rootNodes_.erase(i);
                    (*i)->decrement();
                    break;
                }

                // TODO: look also from children nodes
            }
        }
    }
}
