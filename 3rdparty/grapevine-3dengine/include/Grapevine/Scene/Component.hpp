/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_COMPONENT
#define GRAPEVINE_SCENE_COMPONENT

#include "Grapevine/Scene/AbstractVisitor.hpp"
#include "Grapevine/Scene/AbstractComponentPrivate.hpp"
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class Component
         *
         * Component gives functionality to Nodes. Same Component can be
         * placed to multiple nodes!
         *
         * User can inherit AbstractComponentPrivate to create new
         * functionality to his/her scene's components.
         */
        class Component
        {
        public:
            CLASS_COPYABLE(Component)

            Component(AbstractComponentPrivate* private_);
            virtual ~Component();
            std::string const& type() const;
            bool isNull() const;

            /**
             * Set component to be cumulative for the visitor. Cumulativeness
             * can be used for example to stack or/and group
             * values specific to component over the parent-children
             * hierarchy.
             *
             * @note as default value is true.
             *
             * @param [in]  cumulativeOn    true for on, false for off
             */
            void setCumulative(bool value);

            /**
             * @return cumulative. Returned value tells is the current
             *      component values affected by component parent node's
             *      component values.
             */
            bool cumulative();

        protected:
            friend class NodePrivate;
            ReferencedPointer<AbstractComponentPrivate> private_;

        private:
            Component();
        };

    }
}

#endif
