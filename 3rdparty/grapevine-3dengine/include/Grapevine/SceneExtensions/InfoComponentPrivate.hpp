/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_INFOCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_INFOCOMPONENTPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class InfoComponentPrivate: public AbstractComponentPrivateTemplate<InfoComponentPrivate>
            {
            public:
                CLASS_UNCOPYABLE(InfoComponentPrivate)

                InfoComponentPrivate(std::string const& name);
                ~InfoComponentPrivate();

                void setName(std::string const& name);
                std::string const& name();

            private:
                std::string name_;
            };
        }
    }
}

#endif
