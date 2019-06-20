// author: Zhiliang Zhou

#include "state_machine.h"
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

StateMachine::StateMachine(uint8_t max_states, uint8_t initial_state) :
	MAX_STATES(max_states),
	m_current_state(initial_state),
	m_new_state(EVENT_IGNORED),
	m_event_generated(EVENT_IGNORED)
{
	assert(MAX_STATES < EVENT_IGNORED);
}  


void StateMachine::invokeExternalEvent(uint8_t new_state, EventDataPtr pdata){
	// If we are supposed to ignore this event
	if (new_state == EVENT_IGNORED){
        //just ignore
	}else{
		// TODO - capture software lock here for thread-safety if necessary
		// Generate the event
		invokeInternalEvent(new_state, pdata);

		// Execute the state engine. This function call will only return
		// when all state machine events are processed.
		StateEngine();
		// TODO - release software lock here 
	}
}

// update to new state
// why need to save event data to buffer ?
void StateMachine::invokeInternalEvent(uint8_t new_state, EventDataPtr pdata){
	if (pdata == nullptr)
		pdata = std::make_shared<EventData>();

	m_pevent_data = pdata;
	m_event_generated = true;
	m_new_state = new_state;
}

void StateMachine::StateEngine(void){
    const StateMapRow* pstate_map = getStateMap();
    StateEngine(pstate_map);
}

// let new state to run its invokeStateAction()
// if new state is Start, then run ST_start() as its invokeStateAction()
// e.g StateAction<Motor, MotorData, &Motor::ST_start> Start;
void StateMachine::StateEngine(const StateMapRow* const pstate_map){
    EventDataPtr pdata_temp;    
    
    // While events are being generated keep executing states
    while (m_event_generated)
    {
        // Error check that the new state is valid before proceeding
        assert(m_new_state < MAX_STATES);
    
        // Get the pointer from the state map
        StateBase* state = pstate_map[m_new_state].state;
            
        // Copy of event data pointer
        pdata_temp = m_pevent_data;

        // Event data used up, reset the pointer
        m_pevent_data = nullptr;

        // Event used up, reset the flag
        m_event_generated = false;

        // Switch to the new current state
        setCurrentState(m_new_state);

        // Execute the state action passing in event data
        assert(state != nullptr);
        state->invokeStateAction(getptr(), pdata_temp);

        // If event data was used, then delete it
        pdata_temp = nullptr;
    }    
}