/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/Graph.hpp"
#include "GraphPrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        Graph::Graph():
            private_(new GraphPrivate())
        {
        }

        Graph::~Graph()
        {
            delete private_;
        }

        void Graph::accept(AbstractTreeVisitor& visitor)
        {
            private_->accept(visitor);
        }

        void Graph::addNode(Node& node)
        {
            private_->addNode(node.private_.pointer());
        }

        void Graph::remove(Node& node)
        {
            private_->remove(node.private_.pointer());
        }

        template <typename T> std::vector<Node> Graph::nodesWithComponent()
        {
            return private_->nodesWithComponent<T>();
        }
    }
}
