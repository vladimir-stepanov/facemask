/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_STATEMACHINE_STATETRANSITION
#define GRAPEVINE_STATEMACHINE_STATETRANSITION

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/Core/Mutex.hpp>
#include <Grapevine/StateMachine/State.hpp>
#include <string>
using std::ptrdiff_t; // NOTE: sigc++ has problem without this line!
#include <sigc++/signal.h>

namespace Grapevine
{
    /**
     * @class StateTransition
     *
     * Class that implements one state transition of the finite state machine.
     */
    class StateTransition
    {
    public:
        CLASS_UNCOPYABLE(StateTransition)

        /**
         * Emitted if trigger called and transition happens.
         */
        sigc::signal<void> action;

        /**
         * Constructor that creates a screen with given size.
         *
         * @param [in]  event           Event that triggers the state
         *                              transition.
         * @param [in]  from            From what state the transition
         *                              begins from.
         *                              @note State lifetime must be
         *                              longer that StateTransmission using it!
         * @param [in]  to              From what state the transition
         *                              ends to.
         *                              @note State lifetime must be
         *                              longer that StateTransmissions using it!
         */
        StateTransition(std::string const& event, State& from, State& to);
        virtual ~StateTransition();
        std::string const& event() const;
        State* from() const;
        State* to() const;

        /**
         * Function that can be used to check if the transition is
         * possible.
         *
         * @return  FALSE if transition cannot happen.
         *          TRUE if transition can happen.
         */
        virtual bool guard()
        {
            return true;
        }

    private:
        friend class StateMachine;
        std::string event_;
        State* from_;
        State* to_;
    };
}

#endif
