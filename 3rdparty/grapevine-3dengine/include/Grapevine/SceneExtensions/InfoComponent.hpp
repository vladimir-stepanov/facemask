/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_INFOCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_INFOCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <string>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class InfoComponent: public Component
            {
            public:
                CLASS_COPYABLE(InfoComponent)

                InfoComponent(std::string const& name);
                virtual ~InfoComponent();
                void setName(std::string const& name);
                std::string const& name();

            private:
                class InfoComponentPrivate* infoComponentPrivate_;
            };
        }
    }
}

#endif
