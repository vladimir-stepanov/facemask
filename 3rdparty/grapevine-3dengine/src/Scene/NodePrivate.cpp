/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/NodePrivate.hpp"
#include <Grapevine/Config.hpp>
#include <algorithm>

namespace Grapevine
{
    namespace Scene
    {
        NodePrivate::NodesWithComponentVector NodePrivate::nodesWithComponent_;
#ifdef GRAPEVINE_CONFIG_DEBUG
        unsigned int NodePrivate::debugSteps_ = 0;
        float NodePrivate::debugTime_;
        unsigned int NodePrivate::debugStartTime_ = Timer::milliseconds();
        unsigned int NodePrivate::nodeAmount_ = 0;
#endif

        NodePrivate::NodePrivate():
            parent_(NULL),
            depth_(0)
        {
#ifdef GRAPEVINE_CONFIG_DEBUG
            nodeAmount_++;
#endif
        }

        NodePrivate::NodePrivate(Component const& c0):
            parent_(NULL),
            depth_(0)
        {
            addComponent(c0);
#ifdef GRAPEVINE_CONFIG_DEBUG
            nodeAmount_++;
#endif
        }

        NodePrivate::NodePrivate(Component const& c0, Component const& c1):
            parent_(NULL),
            depth_(0)
        {
            addComponent(c0);
            addComponent(c1);
#ifdef GRAPEVINE_CONFIG_DEBUG
            nodeAmount_++;
#endif
        }

        NodePrivate::NodePrivate(Component const& c0, Component const& c1, Component const& c2):
            parent_(NULL),
            depth_(0)
        {
            addComponent(c0);
            addComponent(c1);
            addComponent(c2);
#ifdef GRAPEVINE_CONFIG_DEBUG
            nodeAmount_++;
#endif
        }

        NodePrivate::~NodePrivate()
        {
            for(std::list<NodePrivate*>::iterator i = children_.begin(); i != children_.end(); i++)
            {
                NodePrivate* tmp = *i;
                tmp->parent_ = NULL;
                tmp->decrement();
                tmp = NULL;
            }

            // Remove component ownership
            for(ComponentVector::const_iterator i = components_.begin(); i != components_.end(); ++i)
            {
                std::type_info const& typeId  = *(i->first);
                removeComponentOwner(typeId, this);
            }

            // Components will be removed automatically
#ifdef GRAPEVINE_CONFIG_DEBUG
            nodeAmount_--;
#endif
        }

        void NodePrivate::addComponent(Component const& component)
        {
            // Check if we have this component already
            std::type_info const& typeId = component.private_.pointer()->typeInfo();
            for(ComponentVector::const_iterator i = components_.begin(); i != components_.end(); ++i)
            {
                if(*(i->first) == typeId)
                {
                    // since it's possible to not use template based components, we need to check the type name also
                    if(component.type() == i->second.pointer()->type())
                    {
                        // We had this component already
                        LOGE("NodePrivate::addComponent(), trying to overwrite existing component with type: \"%s\", doing nothing.", component.type().c_str());
                        return;
                    }
                }
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            float tmpTime = Timer::millisecondsFloat();
#endif
            // NEW
            {
                bool foundList2 = false;
                for(NodesWithComponentVector::iterator i = nodesWithComponent_.begin(); i != nodesWithComponent_.end(); ++i)
                {
                    if(*(i->first) == typeId)
                    {
                        ((i->second)).push_back(this);
                        foundList2 = true;
                        break;
                    }
                }
                if(foundList2 == false)
                {
                    std::vector<NodePrivate* > tmp;
                    tmp.push_back(this);
                    nodesWithComponent_.push_back(std::make_pair(&typeId, tmp));
                }
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            debugAddTime(Timer::millisecondsFloat() - tmpTime);
#endif
            // Add to component list
            components_.push_back(std::make_pair(&typeId, component.private_));
            LOGD("addComponent typeId: %s to node id:%d", typeId.name(), id());
            AnimationManager::setSceneDirty();
        }

        void NodePrivate::components(std::vector<ReferencedPointer<AbstractComponentPrivate> > &components)
        {
            ComponentVector::const_iterator i;
            for(i = components_.begin(); i != components_.end(); ++i)
            {
                components.push_back(i->second);
            }
        }

        void NodePrivate::addChild(NodePrivate* node)
        {
            children_.push_back(node);
            node->parent_ = this;
            node->depth_ = depth_ + 1;
            node->increment();
            AnimationManager::setSceneDirty();
        }

        void NodePrivate::removeAllChildren()
        {
            while(children_.size() > 0)
            {
                NodePrivate* tmp = children_.front();
                tmp->parent_ = NULL;
                tmp->decrement();
                tmp = NULL;
                children_.pop_front();
            }
            AnimationManager::setSceneDirty();
        }

        void NodePrivate::removeChild(NodePrivate *node)
        {
            std::list<NodePrivate*>::iterator it = std::find(children_.begin(), children_.end(), node);
            if(it != children_.end())
            {
                children_.erase(it);
                node->parent_ = NULL;
                node->decrement();
                AnimationManager::setSceneDirty();
            }
        }

        NodePrivate* NodePrivate::parent()
        {
            return parent_;
        }

        void NodePrivate::accept(AbstractTreeVisitor& visitor)
        {
#ifdef GRAPEVINE_CONFIG_DEBUG
            float visitTime = Timer::millisecondsFloat();
#endif

            if (visitor.breaking() == false && visitor.canVisit(this) == true)
            {
                visitor.visitNodeEnter(this);
                for (std::list<NodePrivate*>::iterator i = children_.begin(); i != children_.end(); i++)
                {
                    (*i)->privateTreeAccept(visitor);
                }
                visitor.visitNodeLeave(this);
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            visitTime = Timer::millisecondsFloat() - visitTime;
            visitor.debugAddTime(visitTime);
#endif
        }

        void NodePrivate::accept(AbstractListVisitor& visitor)
        {
#ifdef GRAPEVINE_CONFIG_DEBUG
            float visitTime = Timer::millisecondsFloat();
#endif
            if (visitor.breaking() == false && visitor.canVisit(this) == true)
            {
                visitor.visitNodeEnter(this);
                visitor.visitNodeLeave(this);
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            visitTime = Timer::millisecondsFloat() - visitTime;
            visitor.debugAddTime(visitTime);
#endif
        }

        void NodePrivate::privateTreeAccept(AbstractTreeVisitor& visitor)
        {
            if (visitor.breaking() == false && visitor.canVisit(this) == true)
            {
                visitor.visitNodeEnter(this);
                for (std::list<NodePrivate*>::iterator i = children_.begin(); i != children_.end(); i++)
                {
                    (*i)->privateTreeAccept(visitor);
                }
                visitor.visitNodeLeave(this);
            }
        }

        bool NodePrivate::isInsideBranch(NodePrivate* parentCandidate) const
        {
            if(parentCandidate == NULL)
            {
                return false;
            }

            if(parent_ != NULL)
            {
                if((void*)parent_ == (void*)parentCandidate) // TODO: why id() comparison crashes in some cases and this works.
                {
                    return true;
                }
                else
                {
                    return parent_->isInsideBranch(parentCandidate);
                }
            }
            return false;
        }

#ifdef GRAPEVINE_CONFIG_DEBUG
        void NodePrivate::debugPrint()
        {
            LOGD("NodePrivate::debugPrint(), av. from %d ms, ms/ComponentOwnerManagement/frame", DebugTimeMsTrigger);
            LOGD("%*s %.3f", (int)20, "Total:", debugTime_/(float)debugSteps_);
            debugTime_ = 0.0f;
            debugSteps_ = 0;
        }

        void NodePrivate::debugAddTime(float milliseconds)
        {
            debugTime_ += milliseconds;
            debugSteps_++;
            if(Timer::milliseconds() > debugStartTime_ + DebugTimeMsTrigger)
            {
                debugPrint();
                debugSteps_ = 0;
                NodePrivate::debugStartTime_ = Timer::milliseconds();
            }
        }

        unsigned int NodePrivate::nodeAmount()
        {
            return nodeAmount_;
        }
#endif

        void NodePrivate::releaseNodesWithComponentList()
        {
            for(NodesWithComponentVector::iterator i = nodesWithComponent_.begin(); i != nodesWithComponent_.end(); i++)
            {
                nodesWithComponent_.erase(i);
            }
        }

        void NodePrivate::removeComponentOwner(std::type_info const& componentTypeId, NodePrivate* self)
        {
            NodesWithComponentVector::iterator i;
            for(i = nodesWithComponent_.begin(); i != nodesWithComponent_.end(); i++)
            {
                // Component owners exist
                if(*(i->first) == componentTypeId)
                {
                    // Check the vector if Node is found
                    for(std::vector<NodePrivate* >::iterator i2 = i->second.begin(); i2 != i->second.end(); i2++)
                    {
                        NodePrivate* tmp = *i2;
                        if(tmp->id() == self->id())
                        {
                            i->second.erase(i2);

                            // If was last component holder, erase
                            if((i->second).size() == 0)
                            {
                                nodesWithComponent_.erase(i);
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}
