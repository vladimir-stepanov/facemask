/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_NODELISTPRIVATE
#define GRAPEVINE_SCENE_NODELISTPRIVATE

#include "Grapevine/Scene/NodePrivate.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class NodeListPrivate
         */
        class NodeListPrivate: public Referenced
        {
        public:
            CLASS_UNCOPYABLE(NodeListPrivate)

            void accept(AbstractListVisitor& visitor);
            NodeListPrivate(std::vector<ReferencedPointer<NodePrivate> > const& list);
            virtual ~NodeListPrivate();
            static std::vector<ReferencedPointer<NodePrivate> > booleanAnd(NodeListPrivate* a, NodeListPrivate* b);
            static std::vector<ReferencedPointer<NodePrivate> > booleanOr(NodeListPrivate* a, NodeListPrivate* b);
            unsigned int length() const;

        private:
            std::vector<ReferencedPointer<NodePrivate> > list_;
        };
    }
}

#endif
