/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_INPUT_INPUTSOURCEPRIVATENGOS
#define GRAPEVINE_INPUT_INPUTSOURCEPRIVATENGOS

#include <Grapevine/Core/Mutex.hpp>

#include "InputSourcePrivate.hpp"
#include "InputSourceThread.hpp"

namespace Grapevine
{
    class InputSourceThread;
    class InputSourcePrivateNGOS : public InputSourcePrivate
    {
    public:
        InputSourcePrivateNGOS();
        void addEvent(InputEvent* event);
        bool pollEvent(InputEvent& event);
        ~InputSourcePrivateNGOS();

    private:
        InputSourcePrivate* private_;
        class PowerThread : public InputSourceThread { /* power button */
        public:
            PowerThread(void *parent, inputSourceThreadCb *cb) : InputSourceThread(parent, cb) {}
        private:
            void run();
        };
        class VolumeThread : public InputSourceThread { /* volume button */
        public:
            VolumeThread(void *parent, inputSourceThreadCb *cb) : InputSourceThread(parent, cb) {}
        private:
            void run();
        };
        class TouchThread : public InputSourceThread { /* touch-screen */
        public:
            TouchThread(void *parent, inputSourceThreadCb *cb) : InputSourceThread(parent, cb) {}
        private:
            void run();
        };

        PowerThread powerThread_;
        VolumeThread volumeThread_;
        TouchThread touchThread_;
        Mutex mutex_;
        std::queue<InputEvent> queue_;
    };
}

#endif
