/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_NODE
#define GRAPEVINE_SCENE_NODE

#include "Grapevine/Scene/AbstractVisitor.hpp"
#include "Grapevine/Scene/Component.hpp"
#include "Grapevine/Scene/NodeList.hpp"
#include "Grapevine/Scene/NodePrivate.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    namespace Scene
    {
        class ComponentPrivate;

        /**
         * @class Node
         *
         * Node is a class to create tree like structures with. Each node
         * and it's placement in the tree is unique. This is the major
         * difference to Component class.
         *
         * @note Node can be only child to Graph or other Node.
         *       TODO: currently there is no sanity check if user missuses
         *       the tree structure for example by placing the same node
         *       into multiple places!
         */
        class Node
        {
        public:
            CLASS_COPYABLE(Node)

            /**
             * Constuctors for Node with different number of components.
             * @note more N component constuctors can be done when needed.
             */
            Node();
            Node(Component const& c0);
            Node(Component const& c0, Component const& c1);
            Node(Component const& c0, Component const& c1, Component const& c2);
            Node(NodePrivate *priv);

            virtual ~Node();

            /**
             * Accept visitor.
             *
             * @param [in]  visitor     Visitor visiting the node.
             */
            void accept(AbstractTreeVisitor& visitor);

            /**
             * Accept visitor.
             *
             * @param [in]  visitor     Visitor visiting the node.
             */
            void accept(AbstractListVisitor& visitor);

            /**
             * Add child to node.
             *
             * @param [in]  node    Node to add as child.
             */
            void addChild(Node const& node);

            /**
             * Removes all children from node.
             */
            void removeAllChildren();

            /**
             * Removes children from node.
             */
            void removeChild(Node node);

            /**
             * Add component to node.
             *
             * @param [in]  node    AbstractNode to add to scene.
             */
            void addComponent(Component const& component);

            /**
             * Remove component from node.
             *
             * @param [in]  type    Component type.
             */
            template<class T> void removeComponent()
            {
                private_.pointer()->removeComponent<T>();
            }

            /**
             * Get component from node.
             *
             * @note Look component() function. This is a template
             *       version from the same function.
             *
             * @param [in]  type    Component type.
             */
            template<class T> ReferencedPointer<T> component()
            {
                return private_.pointer()->component<T>();
            }

            /**
            * Compare node to node.
            *
            * @param [in]  node    Node.
            */
            bool equals(Node node);

            template<class T> static NodeList componentOwners()
            {
                return NodeList(NodePrivate::componentOwners<T>());
            }

        private:
            friend class Graph;
            ReferencedPointer<NodePrivate> private_;
        };
    }
}

#endif
