/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/StateMachine/StateMachine.hpp>

namespace Grapevine
{
    StateMachine::StateMachine(std::vector<State*>& states, std::vector<StateTransition*>& transitionTable):
        states_(states),
        transitionTable_(transitionTable),
        currentState_(NULL)
    {
        // Check that we have Initial and Final states
        bool hasInitial = false;
        bool hasFinal = false;
        for (std::vector<State*>::iterator i = states_.begin(); i != states_.end(); i++)
        {
            State* tmp = *i;
            if (tmp->isInitial() == true)
            {
                hasInitial = true;
                currentState_ = tmp;
            }
            if (tmp->isFinal() == true)
            {
                hasFinal = true;
            }
        }
        if (hasInitial == false || hasFinal == false)
        {
            throw Exception("StateMachine, Initial and Final states not given.");
        }
    }

    StateMachine::~StateMachine()
    {
        //delete private_;
    }

    void StateMachine::processEvent(std::string const& event)
    {
        std::vector<StateTransition*> possibleTransitions;
        for (std::vector<StateTransition*>::iterator i = transitionTable_.begin(); i != transitionTable_.end(); i++)
        {
            StateTransition* tmp = *i;
            if (tmp->event() == event && tmp->from() == currentState() && tmp->guard() == true)
            {
                possibleTransitions.push_back(tmp);
            }
        }

        // Process possible transitions
        if (possibleTransitions.size() == 1)
        {
            StateTransition* tmp = possibleTransitions[0];
            tmp->from_->onExit(event);
            tmp->action.emit();
            currentState_ = tmp->to_;
            tmp->to_->onEntry(event);
            stateChanged_.emit();
        }
        else if (possibleTransitions.size() == 0)
        {
            LOGI("Event \"%s\" from state \"%s\" has no transitions, do nothing.",
                event.c_str(), currentState_->name().c_str());
        }
        else // if (possibleTransitions.size() > 1)
        {
            LOGE("Event \"%s\" from state \"%s\" has more than one possible transitions, do nothing.",
                event.c_str(), currentState_->name().c_str());
        }

    }

    State const* StateMachine::currentState() const
    {
        return currentState_;
    }
}
