/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_STATEMACHINE_STATEMACHINE
#define GRAPEVINE_STATEMACHINE_STATEMACHINE

#include <Grapevine/Core/Class.hpp>
#include <Grapevine/StateMachine/State.hpp>
#include <Grapevine/StateMachine/StateTransition.hpp>
#include <vector>

namespace Grapevine
{
    /**
     * @class StateMachine
     *
     * Class that implements finite state machine functionality.
     */
    class StateMachine
    {
    public:
        CLASS_UNCOPYABLE(StateMachine)

        /**
         * Constructor that creates a screen with given size.
         *  @note States and StateTransmissions lifetime must be
         *        longer that the StateMachine's using them!
         *
         * @param [in]  states          All possible states.
         *                              @note remember to have one
         *                              StateInitial and one StateFinal listed!
         * @param [in]  transitionTable All possible state transitions.
         *                              @note Currently does not has any
         *                              sanity check from the transition connections.
         *                              @note State lifetime must be
         *                              longer that StateTransmissions using it!
         */
        StateMachine(std::vector<State*>& states, std::vector<StateTransition*>& transitionTable);
        ~StateMachine();
        void processEvent(std::string const& event);
        State const* currentState() const;

        sigc::signal<void> stateChanged_;

    private:
        std::vector<State*> states_;
        std::vector<StateTransition*> transitionTable_;
        State* currentState_;
    };
}

#endif
