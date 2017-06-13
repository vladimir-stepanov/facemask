/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENEEXTENSIONS_ANIMATIONCOMPONENTPRIVATE
#define GRAPEVINE_SCENEEXTENSIONS_ANIMATIONCOMPONENTPRIVATE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Scene/AbstractComponentPrivateTemplate.hpp>
#include <Grapevine/Animation/Animation.hpp>
#include <Grapevine/Math/Vector.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            class AnimationComponentPrivate: public AbstractComponentPrivateTemplate<AnimationComponentPrivate>
            {
            public:
                sigc::signal<void> done_;

                CLASS_UNCOPYABLE(AnimationComponentPrivate)

                AnimationComponentPrivate(Animation const& animation, bool autoRemove = false);
                ~AnimationComponentPrivate();
                void start();
                bool done();
                void emitDone();
                Vector3 translation() const;
                Matrix4 rotation() const;
                Vector3 scale() const;
                float opacity() const;
                float progress();
                bool autoRemove() const;

            private:
                friend class AnimatorVisitor;
                friend class ListAnimatorVisitor;
                Grapevine::Animation animation_;
                bool doneEmitted_;
                bool autoRemove_;
            };
        }
    }
}

#endif
