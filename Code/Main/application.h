
#ifndef _APP_
#define _APP_

#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <string>

component Application : public TypeII {

public:

	int srcID;																								//Source id of the application. DL case, AP // UL case, STA
	int destID;																								//Destination id of the traffic. DL case, STA // UL case, AP
	std::string TProfile;																			//Traffic profile: STREAMING or ELASTIC

public:

	//Constructor and destructor of instances
	Application();
	~Application();

	// COST
	void Setup();
	void Start();
	void Stop();

	//Inports
	inport void inCtrlAP (AppCTRL &n);											//Inport to receive messages from the AP

	//Outports
	outport void outCtrlAP (AppCTRL &n);										//Outport to send messages to the AP

	//Timers and functions to trigger
	Timer <trigger_t> start;
	Timer <trigger_t> end;

	inport inline void Start(trigger_t&);										//Function to start a new flow from the source ID to the destination ID
	inport inline void End(trigger_t&);											//Function to end an existing flow from the source ID to the destination ID
};

Application::Application(){

	connect start.to_component,Start;
	connect end.to_component,End;
}

Application::~Application(){

}

void Application::Setup(){

	//Do nothing
}

void Application::Start(){

	//Do nothing
}

void Application::Stop(){

	//Do nothing
}

/* ----------------------------------------------------------------------------------
This function is used to start new flows. If the type set is Elastic, the flow is set
to be finished upon an Exponential(t_EndFlow). Otherwise, the flow is set as ACTIVE
during the whole simulation time.
---------------------------------------------------------------------------------- */

void Application::Start(trigger_t&){

	//Start flow
	AppCTRL ctrl("FLOW_START", srcID, destID);
	outCtrlAP(ctrl);

	/* The Application controls streaming flows,
	since we need triggers to indicate their end.*/
	if (TProfile.compare("STREAMING") != 0){
		end.Set(SimTime()+Exponential(t_EndFlow));
	}
	else{
		end.Set(runTime-(1E-3));
	}
}

/* ----------------------------------------------------------------------------------
This function is used to end an existing flow.
---------------------------------------------------------------------------------- */

void Application::End(trigger_t&){

	AppCTRL ctrl("FLOW_END", srcID, destID);
	outCtrlAP(ctrl);

	//Schedule next arrival
	start.Set(SimTime()+Exponential(t_ActFlow));
}

/* ----------------------------------------------------------------------------------
This function controls the messages sent by the AP to control the Application:
- CTRL_START -> A new station has been associated and it is thriving for data.
- CTRL_CANCEL -> An station has been deattached from the AP. Only can be performed
if an station does not have any ongoing flow.
---------------------------------------------------------------------------------- */

void Application::inCtrlAP (AppCTRL &m){

	if (m.getDestination() == destID){

		std::string type = m.getType();

		if (type.compare("CTRL_START") == 0){

			srcID = m.getSender();
			TProfile = m.getTProfile();

			if ((TProfile.compare("STREAMING") == 0)||(TProfile.compare("ELASTIC") == 0)){
				start.Set(SimTime()+Exponential(t_ActFlow));
			}
		}
		else if (type.compare("CTRL_CANCEL") == 0){
			if (start.Active())
				start.Cancel();
		}
	}
}

#endif
