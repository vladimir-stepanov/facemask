/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "ListAnimatorVisitor.hpp"
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
            ListAnimatorVisitor::ListAnimatorVisitor():
                Scene::AbstractListVisitor("ListAnimatorVisitor")
            {
            }

            void ListAnimatorVisitor::visitNodeEnter(NodePrivate* nodePrivate)
            {
                // Define current node's opacity (either animated or then model/text one (model preferred over text!)
                ReferencedPointer<AnimationComponentPrivate> animationComponent = nodePrivate->component<AnimationComponentPrivate>();
                AnimationComponentPrivate* animationComponentPrivate = animationComponent.pointer();
                if (animationComponentPrivate != NULL)
                {
                    // Animate
                    bool done = animationComponentPrivate->done();
                    if (!done)
                    {
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
            }

            void ListAnimatorVisitor::visitNodeLeave(NodePrivate* nodePrivate)
            {
            }
        }
    }
}
