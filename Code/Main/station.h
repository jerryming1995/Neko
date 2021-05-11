#ifndef _STA_
#define _STA_

component STA : public TypeII {

public:

	int staID;																						//General identifier for the node
	int servingAP;																				//Identifier of the serving AP
	std::string state;																		//Whether an station is idle (no flow receiving) or busy (receiving flow)
	std::string traffic_type;															//Type of traffic that the station requires from the Ap

	Position coordinates;																	//coordinates of the node.
	Configuration configuration;													//configuration of the node.
	Capabilities capabilities;														//capabilities of the node.

	std::vector<STAInterface> InterfaceContainer;					//Interfaces of the node.
	std::vector<WifiAP> InRangeAPs;												//Vector to store the inrange APs detected by the initial PROBE_REQ
	std::deque <Notification> Queue;

	Flow flow;																						//Flow transmitted by the AP.
	STAStatistics statistics;															//Statistics gathered at the station.

public:

	STA();
	~STA();

	void Setup();																					//COST libary method
	void Start();																					//COST libary method
	void Stop();																					//COST libary method

	//Inports
	inport void inCrtlAP(Notification &n);								//Control channel from Aps to communicate with stations.
	inport void inDataAP(Flow &f);												//Data channel to receive flows from AP.

	//Outports
	outport void outCtrlAP(Notification &n);							//Control channel to APs to communicate control messages.

	//Timers and functions to trigger
	//Timer <trigger_t> DAPS;																//Trigger that fires the dynamic access point selection.
	//inport inline void DAPSByLearning(trigger_t&);				//Function that performs the learning process and the dynamic access point selection.

	Timer <trigger_t> WaitProbes;
	inport inline void Discovery(trigger_t&);

	//Other member functions
	void Initialization();																//Function that creates interfaces. The initial configuration is requested to the serving AP.
	void NotifyAP(std::string, std::vector<double> *v);
	void DoMLOSetup(Notification &n);
	void UpdateInterfaces(Notification &n);
	void CalculateStats();
};

STA::STA (){
	connect WaitProbes.to_component,Discovery;
	//connect DAPS.to_component,DAPSByLearning;
}

STA::~STA (){
}

void STA::Setup(){

}

void STA::Start(){
	Initialization();
}

void STA::Stop(){
}

/* ----------------------------------------------------------------------------------
Upon start, the STA is initialized by creating the interfaces. The fc and Bw selection
will be later configured by the serving AP.

- If MLO enabled, the number of interfaces depends on the maximum set by the config
file. Otherwise, only one is considered.s
---------------------------------------------------------------------------------- */

void STA::Initialization(){

	if (capabilities.Multilink){
		int intSz = maxIntNum;
		for (int i=0; i<intSz; i++){
			STAInterface interface;
			interface.id = i;
			InterfaceContainer.push_back(interface);
		}
	}
	else{
		STAInterface interface;
		interface.id = 0;
		interface.active = true;
		InterfaceContainer.push_back(interface);
	}
	NotifyAP("PROBE_REQ", nullptr);
	NotifyAP("CONFIG_REQ", nullptr);
}

/* ----------------------------------------------------------------------------------
Function to notify different control messages to the AP:
- PROBE_REQ -> To construct the In range AP set.
- CONFIG_REQ -> To request configuration from the serving AP for the association link
- MLO_SETUP_REQ -> To trigger the MLO configuration for all the interfaces
- UPDATE_MLO -> In case of association change, the update MLO triggers a reconfiguration
process for all interfaces.
---------------------------------------------------------------------------------- */

void STA::NotifyAP(std::string type, std::vector<double> *v){

	if (type.compare("PROBE_REQ") == 0){
		Notification notification ("PROBE_REQ", staID, -1);
		outCtrlAP(notification);
	}
	else if (type.compare("CONFIG_REQ") == 0){
		Notification notification ("CONFIG_REQ", staID, servingAP);
		outCtrlAP(notification);
	}
	else if (type.compare("MLO_SETUP_REQ") == 0){
		Notification notification ("MLO_SETUP_REQ", staID, servingAP);
		notification.setLinkQuality(*v);
		outCtrlAP(notification);
	}
	else if (type.compare("UPDATE_MLO") == 0){
		Notification notification ("UPDATE_MLO", staID, servingAP);
		notification.setLinkQuality(*v);
		outCtrlAP(notification);
	}
}

/* ----------------------------------------------------------------------------------
Function that receives the different control messages from the AP:
- PROBE_RESP -> All APs send the response message to the STA, which constructs the
set of nearby APs.
- CONFIG_RESP -> The serving AP asnwers back with the config for the association link.
Upon this message, if the STA is MLO capable calculates the linkQ and sends it back
to perform the MLO configuration.
- MLO_SETUP_RESP -> After evaluating the linkQ, the AP sends back the links available
to perform the MLO operation.
- CHANNEL_SWITCH_STA -> If the AP changes its configuration, it notifies the sta.
- SAT_UPDATE -> The Ap notifies the station with any change, so the flow satisfaction
can be updated by the station.
- FLOW_END -> The ongoing flow finished, and the statistics can be collected.
---------------------------------------------------------------------------------- */

void STA::inCrtlAP(Notification &n){

	if (n.getDestination() == staID){
		std::string type = n.getType();
		if (type.compare("PROBE_RESP") == 0){
			Queue.push_back(n);
			if (!WaitProbes.Active()){
				WaitProbes.Set(SimTime());
			}
		}
		else if (type.compare("CONFIG_RESP") == 0){
			if (!capabilities.Multilink){
				std::vector<double> fc = n.getFc();
				InterfaceContainer.at(0).fc = fc.at(0);
			}
			else{
				DoMLOSetup(n);
			}
		}
		else if (type.compare("MLO_SETUP_RESP") == 0){

			for (int i=0; i<(int)InterfaceContainer.size(); i++){
				InterfaceContainer.at(i).active = false;
			}

			std::vector<double> LinkFc = n.getFc();
			for (int i=0; i<(int)LinkFc.size(); i++){
				std::string band = GetBand(LinkFc.at(i));
				if (band.compare("2_4GHz") == 0){
					InterfaceContainer.at(i).fc = LinkFc.at(i);
					InterfaceContainer.at(i).active = true;
				}
				else if (band.compare("5GHz") == 0){
					InterfaceContainer.at(i).fc = LinkFc.at(i);
					InterfaceContainer.at(i).active = true;
				}
				else if (band.compare("6GHz") == 0){
					InterfaceContainer.at(i).fc = LinkFc.at(i);
					InterfaceContainer.at(i).active = true;
				}
			}
		}
		else if(type.compare("CHANNEL_SWITCH_STA") == 0){

			for (int i=0; i<(int)InterfaceContainer.size(); i++){
				InterfaceContainer.at(i).active = false;
			}
			UpdateInterfaces(n);
		}
		else if ((type.compare("SAT_UPDATE") == 0) && (state.compare("ACTIVE") == 0)){

			std::vector<double> satisfaction = n.getSat();
			flow.setSat(satisfaction);

			std::vector<double> Fc = n.getFc();
			std::vector<double> sat_evolution (InterfaceContainer.size(), 0.0);
			std::vector<double> sim_time (InterfaceContainer.size(), 0.0);

			for (int j=0; j<(int)InterfaceContainer.size(); j++){
				if ((satisfaction.at(j) != -1) && (InterfaceContainer.at(j).active)){
					sat_evolution.at(j) = satisfaction.at(j);
					sim_time.at(j) = SimTime();
					break;
				}
			}
			statistics.SatEvo.push_back(sat_evolution);
			statistics.SimT.push_back(sim_time);
		}
		else if(type.compare("FLOW_END") == 0){
			state = "IDLE";
			CalculateStats();
		}
	}
}

/* ----------------------------------------------------------------------------------
Function that the new flow from the AP to the STA.
---------------------------------------------------------------------------------- */

void STA::inDataAP(Flow &f){
	if (f.getDestination() == staID){
		flow = f;
		state = "ACTIVE";
	}
}

/* ----------------------------------------------------------------------------------
Function that constructs the in range APs set after receiving the PROBE_RESP messages
from all the APs in the simulation.
---------------------------------------------------------------------------------- */

void STA::Discovery(trigger_t&){
	while (Queue.size() > 0) {
		Notification n = Queue.front();
		Position AP_coord = n.getPosition();
		std::vector<double> fc = n.getFc();

		double RSSI = CalculateRSSI(configuration.TxPower, fc.at(0), AP_coord.x, AP_coord.y, AP_coord.z, coordinates.x, coordinates.y, coordinates.z);
		if (RSSI >= RSSIth){
			WifiAP ap;
			ap.id = n.getSender();
			ap.coord = AP_coord;
			InRangeAPs.push_back(ap);
		}
		Queue.pop_front();
	}
}

/* ----------------------------------------------------------------------------------
Function that performs the MLO setup, evaluating the linkQ of each reported link by
the AP. Once the linkQ are calculated, a vector is sent to the AP.
---------------------------------------------------------------------------------- */

void STA::DoMLOSetup(Notification &n){

	Position AP_coord = n.getPosition();
	std::vector<double> fc = n.getFc();
	std::vector<double> linkQ;

	for (int i=0; i<(int)fc.size(); i++){
		double RSSI = CalculateRSSI(configuration.TxPower, fc.at(i), AP_coord.x, AP_coord.y, AP_coord.z, coordinates.x, coordinates.y, coordinates.z);
		linkQ.push_back(RSSI);
	}

	NotifyAP("MLO_SETUP_REQ", &linkQ);
}

/* ----------------------------------------------------------------------------------
Function that updates the configuration of each interface upon channel switch event
triggered by the serving AP. If the station is MLO the links are checked in order to
check if they still meet the quality criteria.
---------------------------------------------------------------------------------- */

void STA::UpdateInterfaces(Notification &n){

	if (!capabilities.Multilink){
		std::vector<double> fc = n.getFc();
		InterfaceContainer.at(0).active = true;
		InterfaceContainer.at(0).fc = fc.at(0);
	}
	else{
		for (int i=0; i<(int)InRangeAPs.size(); i++){
			if (servingAP == InRangeAPs.at(i).id){
				std::vector<double> fc = n.getFc();
				std::vector<double> linkQ;

				for (int j=0; j<(int)fc.size(); j++){
					double RSSI = CalculateRSSI(configuration.TxPower, fc.at(j), InRangeAPs.at(i).coord.x, InRangeAPs.at(i).coord.y, InRangeAPs.at(i).coord.z, coordinates.x, coordinates.y, coordinates.z);
					linkQ.push_back(RSSI);
				}
				NotifyAP("UPDATE_MLO", &linkQ);
				break;
			}
		}
	}
}

/* ----------------------------------------------------------------------------------
Function to get the statistics of the ongoing flow.
---------------------------------------------------------------------------------- */

void STA::CalculateStats(){

	double satisfaction = flow.getSatisfaction();
	//double duration = flow.getDuration();
	//double rtx_data = flow.getLength()*flow.getDratio();

	statistics.AvgSatPerFlow.push_back(satisfaction);
	statistics.AvgThPerFlow.push_back(satisfaction*flow.getLength());
	//std::cout << "TO: " << flow.getDestination() << " satisfaction: " << satisfaction << " d_ratio: " << flow.getDratio() << " Length: " << flow.getLength() << " Throughput: " << satisfaction*flow.getLength() << std::endl;
}

#endif
