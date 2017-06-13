/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_NODEPRIVATE
#define GRAPEVINE_SCENE_NODEPRIVATE

#include <Grapevine/Animation/AnimationManager.hpp>
#include "Grapevine/Scene/AbstractTreeVisitor.hpp"
#include "Grapevine/Scene/AbstractListVisitor.hpp"
#include "Grapevine/Scene/Component.hpp"
#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/RecursiveLock.hpp>
#include <Grapevine/Core/Class.hpp>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <typeinfo>
#include <Grapevine/Config.hpp>
#ifdef GRAPEVINE_CONFIG_DEBUG
    #include <map>
#endif

namespace Grapevine
{
    namespace Scene
    {
        class NodePrivate: public Referenced
        {
        public:
            CLASS_UNCOPYABLE(NodePrivate)

            NodePrivate();
            NodePrivate(Component const& c0);
            NodePrivate(Component const& c0, Component const& c1);
            NodePrivate(Component const& c0, Component const& c1, Component const& c2);
            virtual ~NodePrivate();

            void addComponent(Component const& component);
            void addChild(NodePrivate* node);
            void removeAllChildren();
            void removeChild(NodePrivate *node);

            template<class T> void removeComponent()
            {
                RecursiveLock lock(mutexComponents_);

                std::type_info const& typeId = typeid(T);
                //LOGD("removeComponent typeId: %s", typeId.name());

                // TODO: optimize iteration, change to: hash/int id/similar
                ComponentVector::iterator i;
                for(i = components_.begin(); i != components_.end(); ++i)
                {
                    if(*(i->first) == typeId)
                    {
                        components_.erase(i);

                        // Remove from the "component owner" list
                        /*for(NodesWithComponentVector::iterator i2 = NodePrivate::nodesWithComponent_.begin(); i2 != NodePrivate::nodesWithComponent_.end(); ++i2)
                        {
                            if(*(i2->first) == typeId)
                            {
                                std::vector<ReferencedPointer<NodePrivate> >::iterator i3 = i2->second.begin();
                                for(; i3 != i2->second.end(); i3++)
                                {
                                    if((*i3).pointer()->id() == this->id())
                                    {
                                        i2->second.erase(i3);
                                        break;
                                    }
                                }
                            }
                        }*/

                        // NEW
                        for(NodesWithComponentVector::iterator i2 = NodePrivate::nodesWithComponent_.begin(); i2 != NodePrivate::nodesWithComponent_.end(); ++i2)
                        {
                            if(*(i2->first) == typeId)
                            {
                                std::vector<NodePrivate* >::iterator i3 = i2->second.begin();
                                for(; i3 != i2->second.end(); i3++)
                                {
                                    if((*i3)->id() == this->id())
                                    {
                                        i2->second.erase(i3);
                                        break;
                                    }
                                }
                            }
                        }

                        return;
                    }
                }
            }

            void components(std::vector<ReferencedPointer<AbstractComponentPrivate> > &components);
            NodePrivate* parent();
            void accept(AbstractTreeVisitor& visitor);
            void accept(AbstractListVisitor& visitor);

            template<class T> ReferencedPointer<T> component()
            {
                RecursiveLock lock(mutexComponents_);

                // TODO: optimize iteration, change to: hash/int id/similar
                ComponentVector::iterator i;
                for(i = components_.begin(); i != components_.end(); ++i)
                {
                    if(*(i->first) == typeid(T))
                    {
                        return ReferencedPointer<T>(static_cast<T*>(i->second.pointer()));
                    }
                }

                return ReferencedPointer<T>(NULL);
            }

            template<class T> static std::vector<ReferencedPointer<NodePrivate> > componentOwners()
            {
                /*std::type_info const& typeId = typeid(T);
                //LOGD("componentOwners typeId: %s", typeId.name());

                NodesWithComponentVector::iterator i;
                for(i = nodesWithComponent_.begin(); i != nodesWithComponent_.end(); i++)
                {
                    if(*(i->first) == typeId)
                    {
                        return i->second;
                    }
                }

                // If owning nodes not found, return empty vector
                std::vector<ReferencedPointer<NodePrivate> > r;
                return r;
                */

                // NEW
                std::vector<ReferencedPointer<NodePrivate> > r;

                std::type_info const& typeId = typeid(T);

                NodesWithComponentVector::iterator i;
                for(i = nodesWithComponent_.begin(); i != nodesWithComponent_.end(); i++)
                {
                    if(*(i->first) == typeId)
                    {
                        for(std::vector<NodePrivate* >::iterator i2 = i->second.begin(); i2 != i->second.end(); i2++)
                        {
                            NodePrivate* tmp = *i2;
                            r.push_back(ReferencedPointer<NodePrivate>(tmp));
                        }
                        break;
                    }
                }

                // If owning nodes not found, return empty vector
                return r;
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            void debugAddTime(float milliseconds);
            static unsigned int nodeAmount();
#endif
        private:
            friend class GraphPrivate;
            NodePrivate* parent_;

            /**
             * Similar to accept function, used by the accept() internally for children
             */
            unsigned int depth_;
            void privateTreeAccept(AbstractTreeVisitor& visitor);
            RecursiveMutex mutexChildren_;
            std::list<NodePrivate*> children_;
            RecursiveMutex mutexComponents_;
            typedef std::vector<std::pair<std::type_info const*, ReferencedPointer<AbstractComponentPrivate> > > ComponentVector;
            ComponentVector components_;

            /** List of component owners
             * component nodes list order: List will be created so that children
             * are in next indexes compared to parent. By observing node depth,
             * it is possible to make "hiearchical" information stacking
             */
            bool isInsideBranch(NodePrivate* parentCandidate) const;
            static void releaseNodesWithComponentList(); // used by GraphPrivate to release the list

            // NEW
            typedef std::vector<std::pair<std::type_info const*, std::vector<NodePrivate* > > > NodesWithComponentVector;
            static NodesWithComponentVector nodesWithComponent_;
            static void removeComponentOwner(std::type_info const& componentTypeId, NodePrivate* self);

#ifdef GRAPEVINE_CONFIG_DEBUG
            std::string debugName_;
            static void debugPrint();
            static unsigned int debugSteps_;
            static unsigned int debugStartTime_;
            static const unsigned int DebugTimeMsTrigger = 4000;
            static float debugTime_;

            // Amount
            static unsigned int nodeAmount_;
#endif
        };
    }
}

#endif
