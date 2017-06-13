/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_ABSTRACTCOMPONENTPRIVATE
#define GRAPEVINE_SCENE_ABSTRACTCOMPONENTPRIVATE

#include "Grapevine/Scene/AbstractVisitor.hpp"
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Config.hpp>
#include <string>
#include <typeinfo>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class AbstractComponentPrivate
         *
         * Component gives functionality to Nodes. User can inherit
         * AbstractComponentPrivate to create new functionality.
         */
        class AbstractComponentPrivate: public Referenced
        {
        public:
            CLASS_UNCOPYABLE(AbstractComponentPrivate)

            std::string const& type()
            {
                return type_;
            };

            void setCumulative(bool value)
            {
                cumulative_ = value;
            }

            bool cumulative()
            {
                return cumulative_;
            }

            virtual const std::type_info &typeInfo() const
            {
                return typeid(AbstractComponentPrivate);
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            static unsigned int componentAmount()
            {
                return componentAmount_;
            }
#endif
        protected:
            AbstractComponentPrivate(std::string const& type, bool cumulative = true):
                type_(type),
                cumulative_(cumulative)
            {
#ifdef GRAPEVINE_CONFIG_DEBUG
                componentAmount_++;
#endif
            };

            virtual ~AbstractComponentPrivate()
            {
#ifdef GRAPEVINE_CONFIG_DEBUG
                componentAmount_--;
#endif
            };

        private:
            std::string type_;
            bool cumulative_;

#ifdef GRAPEVINE_CONFIG_DEBUG
            static unsigned int componentAmount_;
#endif
        };
    }
}

#endif
