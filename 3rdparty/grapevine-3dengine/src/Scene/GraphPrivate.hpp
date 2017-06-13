/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_GRAPHPRIVATE
#define GRAPEVINE_SCENE_GRAPHPRIVATE

#include "Grapevine/Scene/NodePrivate.hpp"
#include "Grapevine/Scene/Node.hpp"
#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include <Grapevine/Core/Lock.hpp>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class Graph
         *
         * Graph is a class to collect nodes to.
         */
        class GraphPrivate
        {
        public:
            void accept(AbstractTreeVisitor& visitor);

            /**
             * Add node to graph.
             *
             * @param [in]  node    AbstractNode to add to scene.
             */
            void addNode(NodePrivate* node);

            /**
             * Add node to graph.
             *
             * @param [in]  node    AbstractNode to remove from scene.
             */
            void remove(NodePrivate* node);

            /**
             * NOTE: under testing function for "system component list" like functionality.
             */
            template <typename T> std::vector<Node> nodesWithComponent()
            {
                Node tmp;
                std::vector<Node> r;
                r.push_back(tmp);
                return r;
            }

        private:
            friend class Graph;

            GraphPrivate();
            ~GraphPrivate();

            Mutex mutex_;
            std::list<NodePrivate*> rootNodes_;
        };
    }
}

#endif
