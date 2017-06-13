/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#include <string>
#include <Grapevine/StateMachine/State.hpp>

namespace Grapevine
{
    State::State(std::string const& name):
        name_(name),
        type_(Type::Normal)
    {
    }

    State::~State()
    {
    }

    bool State::isInitial() const
    {
        return (type_ == Type::Initial);
    }

    bool State::isFinal() const
    {
        return (type_ == Type::Final);
    }

    std::string const& State::name() const
    {
        return name_;
    }

    bool State::operator==(const State& r) const
    {
        if (this == &r)
        {
            return true;
        }
        return false;
    }
}
