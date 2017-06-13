//!

#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/StateMachine/State.hpp>
#include <Grapevine/StateMachine/StateTransition.hpp>
#include <Grapevine/StateMachine/StateMachine.hpp>

using namespace Grapevine;

class MyState0: public State
{
public:
    MyState0(std::string const& name):
        State(name)
    {
    }
};

class MyState1: public State
{
public:
    MyState1(std::string const& name):
        State(name)
    {
    }
};

class MyState2: public State
{
public:
    MyState2(std::string const& name):
        State(name)
    {
    }

    virtual void onExit(std::string const& event)
    {
        LOGI("MyState2 \"%s\" exit", name().c_str());
    }

    virtual void onEntry(std::string const& event)
    {
        LOGI("MyState2 \"%s\" entry", name().c_str());
    }
};

int main(int argc, char *argv[])
{
    StateInitial stateInitial;
    MyState0 state0("state0");
    MyState1 state1("state1");
    MyState2 state2("state2");
    StateFinal stateFinal;

    StateTransition transition0("event0", stateInitial, state0);
    StateTransition transition1("event1", state0, state1);
    StateTransition transition2("event2", state1, state2);
    StateTransition transition3("event3", state2, stateFinal);

    std::vector<State*> states;
    states.push_back(&stateInitial);
    states.push_back(&state0);
    states.push_back(&state1);
    states.push_back(&state2);
    states.push_back(&stateFinal);

    std::vector<StateTransition*> transitions;
    transitions.push_back(&transition0);
    transitions.push_back(&transition1);
    transitions.push_back(&transition2);
    transitions.push_back(&transition3);

    LOG
    StateMachine stateMachine(states, transitions);

    // Should print error:
    stateMachine.processEvent("event_NOT_EXISTING");

    // These are working OK:
    stateMachine.processEvent("event0");
    stateMachine.processEvent("event1");
    stateMachine.processEvent("event2");
    stateMachine.processEvent("event3");

    return 0;
}

