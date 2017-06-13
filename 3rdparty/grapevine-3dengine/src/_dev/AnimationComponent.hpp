/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ANIMATIONCOMPONENT
#define GRAPEVINE_SCENEEXTENSIONS_ANIMATIONCOMPONENT

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/Component.hpp>
#include <Grapevine/Animation/Animation.hpp>
#include <string>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AnimationComponent: public Component
            {
            public:
                sigc::signal<void> done_;

                CLASS_COPYABLE(AnimationComponent)

                // Now just PathAnimation supported. NOTE: more when needed
                AnimationComponent(Animation const& animation, bool autoRemove = false);
                virtual ~AnimationComponent();

                // TODO: add more controls
                void start();
                bool done();

            private:
                class AnimationComponentPrivate* animationComponentPrivate_;
            };
        }
    }
}

#endif
