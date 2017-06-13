/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include "Grapevine/Scene/AbstractVisitor.hpp"
#include "Grapevine/Core/Timer.hpp"
#include <Grapevine/Config.hpp>
#include <vector>
#include <algorithm>    // std::sort

namespace Grapevine
{
    namespace Scene
    {
#ifdef GRAPEVINE_CONFIG_DEBUG
        unsigned int AbstractVisitor::debugSteps_ = 0;
        unsigned int AbstractVisitor::longestVisitorNameForDebugPrintLayout_ = 0;
        std::map<std::string, float> AbstractVisitor::debugTimes_;
        unsigned int AbstractVisitor::debugStartTime_ = Timer::milliseconds();
#endif

        AbstractVisitor::AbstractVisitor(std::string const& name):
            Referenced(),
            break_(false)
#ifdef GRAPEVINE_CONFIG_DEBUG
            ,
            debugName_(name)
#endif
        {
#ifdef GRAPEVINE_CONFIG_DEBUG
            if(longestVisitorNameForDebugPrintLayout_ < debugName_.length())
            {
                longestVisitorNameForDebugPrintLayout_ = debugName_.length();
            }
#endif
        }

        AbstractVisitor::~AbstractVisitor()
        {
        }

#ifdef GRAPEVINE_CONFIG_DEBUG
        void AbstractVisitor::debugStep()
        {
            debugSteps_++;
            if(Timer::milliseconds() > debugStartTime_ + DebugTimeMsTrigger)
            {
                debugPrint();
                debugSteps_ = 0;
                AbstractVisitor::debugStartTime_ = Timer::milliseconds();
            }
        }

        bool sortDebugTimesFunction(std::pair<std::string, float> first, std::pair<std::string, float> second)
        {
            if(first.second < second.second)
            {
                return false;
            }
            return true;
        }

        void AbstractVisitor::debugPrint()
        {
            LOGD("AbstractVisitor::debugPrint(), av. from %d ms, ms/Visitor/frame", DebugTimeMsTrigger);

            // Create tmp vector for sort
            std::vector<std::pair<std::string, float> > tmpVector(debugTimes_.begin(), debugTimes_.end());
#ifdef __LINUX__
            std::sort(tmpVector.begin(), tmpVector.end(), &sortDebugTimesFunction);
#endif
            // Print times in descending order
            float total = 0.0f;
            for(std::vector<std::pair<std::string, float> >::iterator i = tmpVector.begin();
                i != tmpVector.end(); i++)
            {
                std::string id = i->first;
                float time = i->second;
                LOGD("%*s: %.3f", (int)longestVisitorNameForDebugPrintLayout_, id.c_str(), time/(float)debugSteps_);
                total += i->second;
            }
            LOGD("-----------------------------------------------");
            LOGD("%*s %.3f", (int)longestVisitorNameForDebugPrintLayout_, "Total:", total/(float)debugSteps_);

            // Zero times
            for(std::map<std::string, float>::iterator i = debugTimes_.begin(); i != debugTimes_.end(); i++)
            {
                i->second = 0.0f;
            }
            debugSteps_ = 0;
        }

        void AbstractVisitor::debugAddTime(float milliseconds)
        {
            std::map<std::string, float>::iterator i = debugTimes_.find(debugName_);
            if(i != debugTimes_.end())
            {
                i->second += milliseconds;
            }
            else
            {
                debugTimes_[debugName_] = milliseconds;
            }
        }
#endif
    }
}
