/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_ABSTRACTCOMPONENTTEMPLATEPRIVATE
#define GRAPEVINE_SCENE_ABSTRACTCOMPONENTTEMPLATEPRIVATE

#include <Grapevine/Scene/AbstractComponentPrivate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        /**
         * @class AbstractComponentPrivateTemplate
         *
         * Component gives functionality to Nodes. User can inherit
         * AbstractComponentPrivateTemplate to create new functionality.
         */
        template<class T> class AbstractComponentPrivateTemplate: public AbstractComponentPrivate
        {
        public:
            CLASS_UNCOPYABLE(AbstractComponentPrivateTemplate)

            virtual const std::type_info &typeInfo() const
            {
                return typeid(T);
            }

        protected:
            AbstractComponentPrivateTemplate(std::string const & type):
                AbstractComponentPrivate(type)
            {
            };

            virtual ~AbstractComponentPrivateTemplate()
            {
            };
        };
    }
}

#endif
