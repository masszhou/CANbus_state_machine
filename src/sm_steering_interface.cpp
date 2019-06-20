/**
    state machine for steering control
    Purpose: handel exceptions between ROS and CAN Bus communication for steering control

    @author Zhiliang Zhou
    @version 1.0 12/11/2018
*/

#include "sm_steering_interface.h"
#include <iostream>
using std::cout;
using std::endl;


SteeringInterface::SteeringInterface() :
	StateMachine(ST_MAX_STATES),
    m_gateway_SIState(0),
    m_gateway_GatewayClearanceSI(0),
    m_control_mode(CT_ANGLE)
{
    // build CAN frame
    // dummy
    m_msg_sent = false;
    m_debug_prefix = "ERROR >> SteeringInterface >> ";
    m_driver_seatbelt = 2;
    isDebugMode = false;
}

// action function
// transition invoked by external event
void SteeringInterface::setValue(SteeringInterfaceDataPtr data){
    // follow sequence of
    // STATE_MAP[] = {&Idle, &Standby, &Working, &Stop}; 
    static const uint8_t TRANSITIONS[] = {
        EVENT_IGNORED,         //at state Idle, action setValue() will be ignored
        EVENT_IGNORED,         //at state Clearance, action setValue() will be ignored
        ST_WORKING,            //at state Activation, action setValue() will lead to state ST_WORKING
        ST_WORKING,            //at state Working, action setValue() will lead to state ST_WORKING
        EVENT_IGNORED          //at state Stop, action setValue() will be ignored
    };
    invokeExternalEvent(TRANSITIONS[getCurrentState()], data);
}

void SteeringInterface::stop(){
    static const uint8_t TRANSITIONS[] = {
        EVENT_IGNORED,         //state Idle
        ST_STOP,               //state Clearance
        ST_STOP,               //state Activation
        ST_STOP,               //state Working
        EVENT_IGNORED          //state Stop
    };
    invokeExternalEvent(TRANSITIONS[getCurrentState()], nullptr);
}

void SteeringInterface::sendClearance(){ 
    static const uint8_t TRANSITIONS[] = {
        ST_CLEARANCE,          //state Idle
        EVENT_IGNORED,         //state Clearance
        EVENT_IGNORED,         //state Activation
        EVENT_IGNORED,         //state Working
        EVENT_IGNORED          //state Stop
    };
    invokeExternalEvent(TRANSITIONS[getCurrentState()], nullptr);
}

void SteeringInterface::activate(){ 
    static const uint8_t TRANSITIONS[] = {
        EVENT_IGNORED,          //state Idle
        ST_ACTIVATION,         //state Clearance
        EVENT_IGNORED,         //state Activation
        EVENT_IGNORED,         //state Working
        EVENT_IGNORED          //state Stop
    };
    invokeExternalEvent(TRANSITIONS[getCurrentState()], nullptr);
}

// state functions
void SteeringInterface::ST_idle(const NoEventDataPtr data){
    // idle wait here, send idle message
    // dummy code
    // can frame is ready to send
}

void SteeringInterface::ST_clearance(const NoEventDataPtr data){
    if (isActivationRequirementsOK() == true && hasStopConditions() == false){
        // dummy code, keep counter, CRC, clear data
        if (m_control_mode == CT_ANGLE){
            // dummy code, use angle control
        }else{
            // dummy code, use torque control
        }
        // can frame is ready to send
        // invokeInternalEvent(ST_ACTIVATION);
    }else{
        if (isActivationRequirementsOK() == false)
            cout << "[debug] in standby state, standby requirement FAILED" << endl;
        if (hasStopConditions() == true)
            cout << "[debug] in standby state, stop received" << endl;
        // go to stop, because requirements not fullfilled
        invokeInternalEvent(ST_STOP);
    }
}
// 
void SteeringInterface::ST_activation(const NoEventDataPtr data){
    if (isActivationRequirementsOK() == true && hasStopConditions() == false){
        // dummy code, keep counter, CRC, clear data
        // keep clearance ?
        if (m_control_mode == CT_ANGLE){
            // dummy code, use angle control
        }else{
            // dummy code, use torque control
        }
        // dummy code, send once to activate, no need to keep
        // can frame is ready to send
        // invokeInternalEvent(ST_WORKING);
    }else{
        if (isActivationRequirementsOK() == false)
            cout << "[debug] in standby state, standby requirement FAILED" << endl;
        if (hasStopConditions() == true)
            cout << "[debug] in standby state, stop received" << endl;
        // go to stop, because requirements not fullfilled
        invokeInternalEvent(ST_STOP);
    }
}

void SteeringInterface::ST_working(const SteeringInterfaceDataPtr pdata){
    if (isDebugMode){
        m_gateway_SIState = 2; // inject gateway clearance
    }
    if (isWorkingReqOK() == true && hasStopConditions() == false){
        if (m_gateway_SIState == 2){
            // dummy code, send angle request
            // dummy code, keep clearance for angle control
            // can frame is ready to send
        }else if(m_gateway_SIState == 3){
            // dummy code, send angle request
            // dummy code, keep clearance for torque control
            // can frame is ready to send
        }else{
            cout << "error, logic condition leak" << endl;
            invokeInternalEvent(ST_STOP);
        }
    }else{
        if (isWorkingReqOK() == false)
            cout << "[debug] in working state, working requirement FAILED" << endl;
        if (hasStopConditions() == true)
            cout << "[debug] in working state, stop received" << endl;
        // go to stop, because requirements not fullfilled
        // invokeInternalEvent(ST_STOP);
    }
}

void SteeringInterface::ST_stop(const NoEventDataPtr data){
    // dummy code, keep counter, CRC, clear data
    // can frame is ready to send
    // heartbeats.notify("stop"); // notify all registered observers, SI comes into STOP state

    // transition to Idle via an internal event
    if (m_msg_sent == false){ // use a flag to ensure this state can been sent by update() to CAN BUS, unless stay in this state
        invokeInternalEvent(ST_STOP);
    } else {
        m_msg_sent = false;
        invokeInternalEvent(ST_IDLE);
    }
}

// check preconditions
bool SteeringInterface::isActivationRequirementsOK(){
    // ---- activation requirements
    // 1. Gateway has booted
    // 2. Driver’s door is closed
    // 3. Driver’s seatbelt is fastened
    //*4. Gear lever is in position D, S, N or R
    // 5. Brake pedal is not pressed
    // bla bla bla

    if (isDebugMode){
        return true; // debug
    }else{
        // dummy code, check conditions
        return true;
    }
}

bool SteeringInterface::isWorkingReqOK(){
    // ---- working requirements 
    // 13.after sending 0x110 Steering_Interface::ActivationRequest = 1. 
    // bla bla bla

    if (isDebugMode){
        return true; // debug
    }else{
        // dummy code, check conditions
        return true;
    }
}

bool SteeringInterface::hasStopConditions(){
    // To deactivate the interface (driving manually) via CAN, the user has the following options (SIc):
    // 19. Driver applies a torque to the steering wheel greater than 4 Nm for longer than 500 ms
    // 20. Brake pedal is pressed and the velocity is greater than zero
    // 21. Gear lever is in position P
    // 22. Driver’s seatbelt is unfastened
    // 23. Driver’s door is open
    // bla bla bla
    if (isDebugMode){
        return false; // debug
    }else{
        // dummy code, check conditions
        return false;
    }
}

void SteeringInterface::send_canframe( uint16_t id, uint8_t *bytes, int dlc)
{
    std::string str;
    char idstr[10];
    sprintf(idstr,"0x%02X#",id);
    str = idstr;
    for (int i=0; i<dlc; i++){
    char hexstr[3];
    sprintf(hexstr,"%02X",bytes[i]);
    str += hexstr;
    }
    //dummy->send(can::toframe(str));
}
