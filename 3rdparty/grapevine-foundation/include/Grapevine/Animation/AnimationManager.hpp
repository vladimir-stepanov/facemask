/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_ANIMATION_ANIMATIONMANAGER
#define GRAPEVINE_ANIMATION_ANIMATIONMANAGER

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Mutex.hpp>

namespace Grapevine
{
    class AnimationManager
    {
    public:
        CLASS_UNCOPYABLE(AnimationManager)
        static void setFrameTimestampMilliseconds(unsigned int value);
        static unsigned int frameTimestampMilliseconds();
        static void setSceneDirty(bool value = true);
        static bool sceneDirty();
        static void animationCountAdd();
        static void animationCountReduce();
        static bool animationCountHas();

    private:
        AnimationManager();
        ~AnimationManager();
        static unsigned int timestamp_;
        static bool sceneDirty_;
        static Mutex animationCountMutex_;
        static unsigned int animationCount_;
    };
}

#endif
