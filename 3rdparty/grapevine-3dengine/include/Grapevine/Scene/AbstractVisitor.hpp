/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_SCENE_ABSTRACTVISITOR
#define GRAPEVINE_SCENE_ABSTRACTVISITOR

#include <Grapevine/Core/Referenced.hpp>
#include <Grapevine/Core/Class.hpp>
#include <string>
#include <Grapevine/Config.hpp>
#ifdef GRAPEVINE_CONFIG_DEBUG
    #include <map>
#endif

namespace Grapevine
{
    namespace Scene
    {
        class NodePrivate;

        /**
         * @class AbstractVisitor
         *
         * AbstractVisitor
         */
        class AbstractVisitor: public Referenced
        {
        public:
            CLASS_UNCOPYABLE(AbstractVisitor)


            /**
             * Called when visitor is entering the node.
             */
            virtual void visitNodeEnter(NodePrivate* node) = 0;

            /**
             * Called when visitor is leaving the node.
             */
            virtual void visitNodeLeave(NodePrivate* node) = 0;

            /**
             * Called before entering the node
             */
            virtual bool canVisit(NodePrivate* node)
            {
                return true;
            }

            /**
             * Called when user wants visitor to break out from graph.
             *
             * @note leave() functions will be called normally.
             */
            inline void setBreak()
            {
                break_ = true;
            }

            /**
             * Called when visitor is leaving the node.
             */
            inline bool breaking()
            {
                return break_;
            }

#ifdef GRAPEVINE_CONFIG_DEBUG
            static void debugStep();
            void debugAddTime(float milliseconds);
#endif
        protected:

            /**
             * @param [in]  name    used for debugging purposes. Please use your class name for this.
             */
            AbstractVisitor(std::string const& name);
            virtual ~AbstractVisitor();

        private:
            bool break_;
#ifdef GRAPEVINE_CONFIG_DEBUG
            std::string debugName_;
            static void debugPrint();
            static unsigned int debugSteps_;
            static unsigned int debugStartTime_;
            static const unsigned int DebugTimeMsTrigger = 4000;
            static std::map<std::string, float> debugTimes_;
            static unsigned int longestVisitorNameForDebugPrintLayout_;
#endif
        };
    }
}

#endif
