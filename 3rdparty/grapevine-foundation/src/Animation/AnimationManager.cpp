/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/Animation/AnimationManager.hpp>
#include <Grapevine/Core/Lock.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/Log.hpp>

namespace Grapevine
{
    unsigned int AnimationManager::timestamp_ = Timer::milliseconds();
    bool AnimationManager::sceneDirty_ = false;
    Mutex AnimationManager::animationCountMutex_;
    unsigned int AnimationManager::animationCount_ = 0;

    AnimationManager::AnimationManager()
    {
    }

    AnimationManager::~AnimationManager()
    {
    }

    void AnimationManager::setFrameTimestampMilliseconds(unsigned int value)
    {
        timestamp_ = value;
    }

    unsigned int AnimationManager::frameTimestampMilliseconds()
    {
        return timestamp_;
    }

    void AnimationManager::setSceneDirty(bool value)
    {
        sceneDirty_ = value;
    }

    bool AnimationManager::sceneDirty()
    {
        return sceneDirty_;
    }

    void AnimationManager::animationCountAdd()
    {
        Lock lock(animationCountMutex_);
        animationCount_++;
    }

    void AnimationManager::animationCountReduce()
    {
        Lock lock(animationCountMutex_);
        animationCount_--;
    }

    bool AnimationManager::animationCountHas()
    {
        //LOGD("anim count: %d", animationCount_);
        return animationCount_ > 0;
    }
}
