/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "AnimatorVisitor.hpp"
#include "Grapevine/SceneExtensions/TransformationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/AnimationComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/AnimationProgressTriggerComponentPrivate.hpp"
#include "Grapevine/SceneExtensions/ModelComponentPrivate.hpp"
#include <Grapevine/Scene/NodePrivate.hpp>
#include <Grapevine/SceneExtensions/TextComponentPrivate.hpp>

namespace Grapevine
{
    namespace Scene
    {
        namespace Extensions
        {
            AnimatorVisitor::AnimatorVisitor():
                AbstractTreeVisitor("AnimatorVisitor"),
                active_(false)
            {
            }

            AnimatorVisitor::~AnimatorVisitor()
            {
            }

            void AnimatorVisitor::visitGraphEnter()
            {
                // Do nothing
            }

            void AnimatorVisitor::visitGraphLeave()
            {
                // Do nothing
            }

            void AnimatorVisitor::visitNodeEnter(NodePrivate* nodePrivate)
            {
                bool nodeAnimated = false;

                // Define current node's opacity (either animated or then model/text one (model preferred over text!)
                ReferencedPointer<AnimationComponentPrivate> animationComponent = nodePrivate->component<AnimationComponentPrivate>();
                AnimationComponentPrivate* animationComponentPrivate = animationComponent.pointer();
                if (animationComponentPrivate != NULL)
                {
                    // Animate
                    bool done = animationComponentPrivate->done();
                    if (!done)
                    {
                        nodeAnimated = true;
                        // Transformation
                        ReferencedPointer<AbstractComponentPrivate> transformationComponent = nodePrivate->component("TransformationComponent");
                        if (transformationComponent.pointer() != NULL)
                        {
                            TransformationComponentPrivate* transformationComponentPrivate = static_cast<TransformationComponentPrivate*>(transformationComponent.pointer());
                            transformationComponentPrivate->setLocation(animationComponentPrivate->translation());
                            transformationComponentPrivate->setRotation(animationComponentPrivate->rotation());
                            transformationComponentPrivate->setScale(animationComponentPrivate->scale());
                            transformationComponentPrivate->setOpacity(animationComponentPrivate->opacity());
                        }

                        // Check animation progress trigger
                        ReferencedPointer<AbstractComponentPrivate> animationProgressTriggerComponent = nodePrivate->component("AnimationProgressTriggerComponent");
                        if (animationProgressTriggerComponent.pointer() != NULL)
                        {
                            AnimationProgressTriggerComponentPrivate* animationProgressTriggerComponentPrivate =
                            static_cast<AnimationProgressTriggerComponentPrivate*>(animationProgressTriggerComponent.pointer());
                            if (animationProgressTriggerComponentPrivate->triggerEmitted_ == false &&
                                animationComponentPrivate->animation_.progress() >= animationProgressTriggerComponentPrivate->value_)
                            {
                                LOGD("AnimatorVisitor::visitNodeEnter(), Trigger at progress: %f", animationProgressTriggerComponentPrivate->value_);
                                animationProgressTriggerComponentPrivate->triggerEmitted_ = true;
                                animationProgressTriggerComponentPrivate->trigger.emit();
                            }
                        }
                    }

                    animationComponentPrivate->emitDone(); // emits only when actually done

                    if(done && animationComponentPrivate->autoRemove())
                    {
                        nodePrivate->removeComponent<AnimationComponentPrivate>();
                    }
                }

                active_ |= nodeAnimated;
            }

            void AnimatorVisitor::visitNodeLeave(NodePrivate* nodePrivate)
            {
            }

            bool AnimatorVisitor::active() const
            {
                return active_;
            }
        }
    }
}
