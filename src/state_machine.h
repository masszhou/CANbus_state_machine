/**
    state machine engine
    Purpose: handel exceptions between ROS and CAN Bus communication
	Ref: state machine template was originally learned from David Lafreniere's post

    @author Zhiliang Zhou
    @version 1.0 25/09/2018
*/

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include <cassert>
#include <memory>
#include <iostream>

using std::cout;
using std::endl;
using std::shared_ptr;

class EventData
{
public:
	virtual ~EventData() {}
	//XALLOCATOR
};
typedef EventData NoEventData;
typedef shared_ptr<EventData> EventDataPtr;
typedef shared_ptr<NoEventData> NoEventDataPtr;


class StateMachine;
typedef shared_ptr<StateMachine> StateMachinePtr;

/// @brief Abstract state base class that all states inherit from.
class StateBase
{
public:
	/// Called by the state machine engine to execute a state action. If a guard condition
	/// exists and it evaluates to false, the state action will not execute. 
	/// @param[in] sm - A state machine instance. 
	/// @param[in] data - The event data. 
	virtual void invokeStateAction(StateMachinePtr sm, EventDataPtr data) = 0;
};
typedef shared_ptr<StateBase> StateBasePtr;


template<class SM, class Data, void(SM::*func)(shared_ptr<Data> data)>
class StateAction : public StateBase{
public:
	/// @see StateBase::InvokeStateAction
	virtual void invokeStateAction(StateMachinePtr sm, EventDataPtr data) 
	{
		// Downcast the state machine and event data to the correct derived type
		shared_ptr<SM> derived_sm = std::static_pointer_cast<SM>(sm);
		
		// If this check fails, there is a mismatch between the STATE_DECLARE 
		// event data type and the data type being sent to the state function. 
		// For instance, given the following state defintion:
		//    STATE_DECLARE(MyStateMachine, MyStateFunction, MyEventData)
		// The following internal event transition is valid:
		//    InternalEvent(ST_MY_STATE_FUNCTION, new MyEventData());
		// This next internal event is not valid and causes the assert to fail:
		//    InternalEvent(ST_MY_STATE_FUNCTION, new OtherEventData());
		shared_ptr<Data> derived_data = std::dynamic_pointer_cast<Data>(data);
		assert(derived_data != nullptr);

		// Call the state function
		(derived_sm.get()->*func)(derived_data);
	}
};

/// @brief A structure to hold a single row within the state map. 
struct StateMapRow
{
	StateBase* state;
};


class StateMachine : public std::enable_shared_from_this<StateMachine>
{
public:
    enum { EVENT_IGNORED = 0xFE, CANNOT_HAPPEN };

    StateMachine(uint8_t max_states, uint8_t initial_state = 0);
    virtual ~StateMachine() {}

    uint8_t getCurrentState() { return m_current_state; }
    
protected:
    void invokeExternalEvent(uint8_t new_state, EventDataPtr pdata = nullptr);
    void invokeInternalEvent(uint8_t new_state, EventDataPtr pdata = nullptr);
    
private:
    const uint8_t MAX_STATES;
    uint8_t m_current_state;
    uint8_t m_new_state;
    bool m_event_generated;
    EventDataPtr m_pevent_data;

    virtual const StateMapRow* getStateMap() = 0;
    //virtual const StateMapRowEx* GetStateMapEx() = 0;
    
    void setCurrentState(uint8_t new_state) { m_current_state = new_state; }

    void StateEngine(void);     
    void StateEngine(const StateMapRow* const pstate_map);
    //void StateEngine(const StateMapRowEx* const pStateMapEx);

	std::shared_ptr<StateMachine> getptr() {
        return shared_from_this();
    }
};

#endif /* STATE_MACHINE_H */
