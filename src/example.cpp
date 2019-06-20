/**
    demo for state machine for steering control communication

    @author Zhiliang Zhou
    @version 1.0 20/06/2019
*/

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cmath>

#include "sm_steering_interface.h"

using namespace std;


int main(int argc, char **argv)
{
    // register heartbeats between control units
    // auto fp_ai_test_stop = std::bind(&AccelerationInterface::stop, ai_test);
    // auto fp_si_test_stop = std::bind(&SteeringInterface::stop, si_test);

    // here is only demo/dummy environment
    shared_ptr<SteeringInterface> si_test = make_shared<SteeringInterface>();

    si_test->isDebugMode = true;
    // si_test->heartbeats.registerObserver("stop", fp_ai_test_stop); // si_test should notify its heartbeats state "stop" to others automatically
    
    float value = -0.5;
    SteeringInterfaceDataPtr si_data = make_shared<SteeringInterfaceData>();
    si_data->request = fabs(value);
    si_data->request_sign = (value < 0);

    cout << (int)si_test->getCurrentState() << endl;

    // test invalid operation
    si_test->activate();
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;

    // test invalid operation
    si_test->setValue(si_data);
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;
    
    si_test->sendClearance();
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;
    
    si_test->activate();
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;

    si_test->setValue(si_data);
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;

    si_test->setValue(si_data);
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;

    si_test->stop();
    si_test->sendToCAN();
    cout << (int)si_test->getCurrentState() << endl;
    
    
    return 0;
}