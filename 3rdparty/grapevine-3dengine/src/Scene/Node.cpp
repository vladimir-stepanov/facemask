/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/Node.hpp"
#include "Grapevine/Core/ReferencedPointer.hpp"
#include "Grapevine/Scene/NodePrivate.hpp"

namespace Grapevine
{
    namespace Scene
    {
        Node::Node():
            private_(ReferencedPointer<NodePrivate>(new NodePrivate()))
        {
        }

        Node::Node(Component const& c0):
            private_(ReferencedPointer<NodePrivate>(new NodePrivate(c0)))
        {
        }

        Node::Node(Component const& c0, Component const& c1):
            private_(ReferencedPointer<NodePrivate>(new NodePrivate(c0, c1)))
        {
        }

        Node::Node(Component const& c0, Component const& c1, Component const& c2):
            private_(ReferencedPointer<NodePrivate>(new NodePrivate(c0, c1, c2)))
        {
        }

        Node::Node(Node const& r):
            private_(r.private_)
        {
        }

        Node::Node(NodePrivate *priv)
        : private_(priv)
        {
        }

        Node::~Node()
        {
        }

        Node& Node::operator=(Node const& r)
        {
            if (this != &r)
            {
                private_ = r.private_;
            }
            return *this;
        }

        void Node::accept(AbstractTreeVisitor& visitor)
        {
            private_.pointer()->accept(visitor);
        }

        void Node::accept(AbstractListVisitor& visitor)
        {
            private_.pointer()->accept(visitor);
        }

        void Node::addChild(Node const& node)
        {
            private_.pointer()->addChild(node.private_.pointer());
        }

        void Node::removeAllChildren()
        {
            private_.pointer()->removeAllChildren();
        }

        void Node::removeChild(Node node)
        {
            private_.pointer()->removeChild(node.private_.pointer());
        }

        void Node::addComponent(Component const& component)
        {
            private_.pointer()->addComponent(component);
        }

        bool Node::equals(Node node)
        {
            return (private_.pointer() == node.private_.pointer());
        }
    }
}
