/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_GRAPH
#define GRAPEVINE_SCENE_GRAPH

#include "Grapevine/Scene/Node.hpp"
#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class Graph
         *
         * Graph is a class to collect root nodes to. Difference to
         * Node is that Graph is not holding any Components.
         */
        class Graph
        {
        public:
            CLASS_UNCOPYABLE(Graph)

            Graph();
            virtual ~Graph();

            void accept(AbstractTreeVisitor& visitor);

            /**
             * Add node to graph.
             *
             * @param [in]  node    AbstractNode to add to scene.
             */
            void addNode(Node& node);

            /**
             * Remove node from graph.
             *
             * @param [in]  node    AbstractNode to remove from scene.
             */
            void remove(Node& node);

            /**
             * NOTE: under testing function for "system component list" like functionality.
             */
            template <typename T> std::vector<Node> nodesWithComponent();

        private:
            class GraphPrivate* private_;
        };
    }
}

#endif
