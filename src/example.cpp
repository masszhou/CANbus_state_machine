/**
    demo for state machine for steering control

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

// here is only demo/dummy environment
shared_ptr<SteeringInterface> si_test = make_shared<SteeringInterface>();

void cb_si_activationrequest(const uint8_t& msg){
	switch(msg){
	case 0:
        cout << "steering interface action: stop" << endl;
        si_test->stop();
		break;
	case 1:
        if (si_test->getCurrentState() == SteeringInterface::ST_IDLE) {
            si_test->sendClearance();
            cout << "steering interface action: send clearance" << endl;
        }else if (si_test->getCurrentState() == SteeringInterface::ST_CLEARANCE) {
            si_test->activate();
            cout << "steering interface action: activate" << endl;
        }
		break;
	}
}

void cb_si_steeringanglerequest(const float& msg){
    SteeringInterfaceDataPtr si_data = make_shared<SteeringInterfaceData>();
    si_data->request = fabs(msg);
    si_data->request_sign = (msg < 0); //SteeringAngleRequestSign 0->left, 1->right, SteeringAngleRequest positive->left, negative->right
    si_test->setValue(si_data);
}

int main(int argc, char **argv)
{
    // register heartbeats between control units
    // auto fp_ai_test_stop = std::bind(&AccelerationInterface::stop, ai_test);
    // auto fp_si_test_stop = std::bind(&SteeringInterface::stop, si_test);

    si_test->isDebugMode = true;
    // si_test->heartbeats.registerObserver("stop", fp_ai_test_stop); // si_test should notify its heartbeats state "stop" to others automatically

}