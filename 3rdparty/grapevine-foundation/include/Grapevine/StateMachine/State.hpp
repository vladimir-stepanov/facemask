/**
 * Copyright (C) 2013-2014 Huawei Technologies Finland Oy.
 *
 * This is unpublished work. See README file for more information.
 */

#ifndef GRAPEVINE_STATEMACHINE_STATE
#define GRAPEVINE_STATEMACHINE_STATE

#include <string>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Class.hpp>

namespace Grapevine
{
    class StateInitial;
    class StateFinal;

    /**
     * @class State
     *
     * Class that implements one state of the finite state machine.
     *
     * @note:   You should inherit this class for each state in
     *          your application.
     */
    class State
    {
    public:
        CLASS_UNCOPYABLE(State)

        State(std::string const& name);
        virtual ~State();
        bool isInitial() const;
        bool isFinal() const;
        std::string const& name() const;
        bool operator==(const State& r) const;

        virtual void onExit(std::string const& event)
        {
            LOGI("State \"%s\" exit", name().c_str());
        }

        virtual void onEntry(std::string const& event)
        {
            LOGI("State \"%s\" entry", name().c_str());
        }

    private:
        friend class StateInitial;
        friend class StateFinal;

        struct Type
        {
            enum Enum
            {
                Normal,
                Initial,
                Final
            };
        };

        // Only used by StateInitial and StateFinal
        State(std::string const& name, Type::Enum type):
            name_(name),
            type_(type)
        {
        }

        std::string name_;
        Type::Enum type_;
    };

    /**
     * @class StateInitial
     *
     * Class that implements initial state of the finite state machine.
     */
    class StateInitial: public State
    {
    public:
        StateInitial():
            State("Initial", Type::Initial)
        {
        }
    };

    /**
     * @class StateFinal
     *
     * Class that implements final state of the finite state machine.
     */
    class StateFinal: public State
    {
    public:
        StateFinal():
            State("Final", Type::Final)
        {
        }
    };
}

#endif
