/**
    state machine for steering control
    Purpose: handel exceptions between ROS and CAN Bus communication for steering control

    @author Zhiliang Zhou
    @version 1.0 12/11/2018
*/

#ifndef STEERING_INTERFACE_H
#define STEERING_INTERFACE_H

#include <cassert>
#include "state_machine.h"


class SteeringInterfaceData : public EventData
{
public:
    float request;
    bool request_sign;
};
typedef shared_ptr<SteeringInterfaceData> SteeringInterfaceDataPtr;


// state machine transition refers to ros/src/can_bridge/uml_doc.md
class SteeringInterface : public StateMachine
{
public:
    SteeringInterface();

    // External events taken by this state machine
    // interfaces for user really doing
    void setValue(SteeringInterfaceDataPtr data);
    void stop();
    void sendClearance();
    void activate();

    // automatic internal requirements checking
    bool isActivationRequirementsOK();
    bool isWorkingReqOK();
    bool hasStopConditions();

    // listen from CAN Bus
    // void listenGatewaySIState(uint8_t val);
    // void listenGatewayClearanceSI(bool val);

    enum ControlMode{
        CT_ANGLE,
        CT_TORQUE
    };
    ControlMode m_control_mode;

    // topic, will be sent to socketcan
    // dummy code
    
    enum States{
        ST_IDLE,
        ST_CLEARANCE,
        ST_ACTIVATION,
        ST_WORKING,
        ST_STOP,
        ST_MAX_STATES
    };

    // listen to ROS topics, not quite sure if it is proper to add these functions in this class, reconsider about it later.
    // std::shared_ptr<can::ThreadedSocketCANInterface> m_driver_ptr;
    // void sendToCAN();

    //debug
    int m_driver_seatbelt;

    bool isDebugMode;

    // be careful of loop error, infinite call each other.
    // currently stop() action will be ignored in STOP state, so there is no infinite loop error
    // HeartBeats<string> heartbeats; 

private:
    
    // Define the state machine state functions with event data type
    void ST_idle(const NoEventDataPtr data); // what to do, when enter idle state without event data.
    StateAction<SteeringInterface, NoEventData, &SteeringInterface::ST_idle> Idle; // class derived from StateBase

    void ST_clearance(const NoEventDataPtr data); // what to do, when enter ready state without event data.
    StateAction<SteeringInterface, NoEventData, &SteeringInterface::ST_clearance> Clearance;

    void ST_activation(const NoEventDataPtr data); // what to do, when enter ready state without event data.
    StateAction<SteeringInterface, NoEventData, &SteeringInterface::ST_activation> Activation;

    void ST_working(const SteeringInterfaceDataPtr pdata); // what to do, when enter changevalue state with event data.
    StateAction<SteeringInterface, SteeringInterfaceData, &SteeringInterface::ST_working> Working;

    void ST_stop(const NoEventDataPtr data);
    StateAction<SteeringInterface, NoEventData, &SteeringInterface::ST_stop> Stop;

private:
    virtual const StateMapRow* getStateMap(){
        static const StateMapRow STATE_MAP[] = {&Idle, &Clearance, &Activation, &Working, &Stop};
        assert(sizeof(STATE_MAP)/sizeof(StateMapRow) == ST_MAX_STATES);
        return &STATE_MAP[0]; // pointer to the address of first element
    }
    void send_canframe( uint16_t id, uint8_t *bytes, int dlc);

public:
    uint8_t m_gateway_SIState; // gateway information
    bool m_gateway_GatewayClearanceSI; // gateway information
    bool m_msg_sent; // a flag to mark whether current changes has been sent
    std::string m_debug_prefix;

};

#endif /* STEERING_INTERFACE_H */
