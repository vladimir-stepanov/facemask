/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <Grapevine/StateMachine/StateTransition.hpp>
#include <Grapevine/StateMachine/StateMachine.hpp>

namespace Grapevine
{
    StateTransition::StateTransition(std::string const& event, State& from, State& to):
        event_(event),
        from_(&from),
        to_(&to)
    {
    }

    StateTransition::~StateTransition()
    {
    }

    std::string const& StateTransition::event() const
    {
        return event_;
    }

    State* StateTransition::from() const
    {
        return from_;
    }

    State* StateTransition::to() const
    {
        return to_;
    }
}
