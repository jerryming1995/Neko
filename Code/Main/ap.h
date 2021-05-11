
#ifndef _AP_
#define _AP_

std::default_random_engine gen(rand());

component AP : public TypeII {

public:

	int apID;                                       //AP identifier

	Position coordinates;														//Coordinates (x,y,z) of the node.
	Capabilities capabilities;											//AP's capabilities.
	Configuration configuration;										//AP's configuration.
	PolicyManager policy_manager;										//Ploicy manager that controls the interfaces for Multi-link operation. If MLO disabled, the policy is to TX flows over one interface only.

	std::vector<APInterface> InterfaceContainer;		//Interface container for Multi-link operation. If MLO disabled, this vector only contains 1 position.
	std::vector<WifiAP> NeighboringAPs;							//Vector to store the neighboring APs (inrange APs).
	std::vector<WifiSTA> AssociatedSTAs;						//Vector to store the associated stations.
	std::vector<Flow> OnGoingFlows;									//Queue of ongoing flows.
	//std::vector<Agent> AgentContainer;						//Agent container, in case of multiple agents running in parallel.

	APStatistics statistics;												//Statistics gathered to generate a final report.

public:

	AP();																							//AP Class constructor
	~AP();																						//AP Class destructor

	void Setup();																			//COST libary method
	void Start();																			//COST libary method
	void Stop();																			//COST libary method

	//Inports
	//inport void inDataSTA(Flow &f);									//Data plane to receive data traffic from stas. Only for UL. --> To be implemented <--
	inport void inCtrlApp(AppCTRL &n);								//Control channel from the application associated to each station.
	inport void inCtrlSTA(Notification &n);						//Control channel from stations to communicate any change (configuration, association, etc)
	inport void inCtrlAP(Notification &n);						//Control channel from neighboring APs to communicate changes.
	//inport void inCtrlAgent(AgentNotification &n);		//Control channel from agents that pass new configurations according to the ML target function. --> To be implemented <--

	//Outports
	outport void outDataSTA(Flow &q);									//Data plane to send data traffic to stas.
	outport void outCtrlApp(AppCTRL &n);							//Control channel to the application associated to each station.
	outport void outCtrlSTA(Notification &n);					//Control channel to stations to communicate any change (configuration, association, etc)
	outport void outCtrlAP(Notification &n);					//Control channel to APs to communicate any change (configuration, association, traffic, etc)
	//outport void outCtrlAgent(AgentNotification &n);	//Control channel to agents that pass old configurations and received rewards. --> To be implemented <--

	//Timers and functions to trigger
	//Timer <trigger_t> DCA;														//Trigger that fires the dynamic channel allocation (DCA) mechanism. Only if ML is enabled.
	//inport inline void DCAByLearning(trigger_t&);			//Function that calls the dynamic channel allocation.Only if ML is enabled. --> To be implemented <--

	//Member functions
	void Initialization();														//Function that creates interfaces, agents and sets the initial configuration.
	Flow CreateFlow(int, int, std::string, double);		//When the application fires the activation trigger, a new flow is created.
	void AcceptIncomingFlow(Flow &f);									//Function to accepts a new incoming flow.
	void RegisterAT(Flow &f);													//Function to register the airtime of the new incoming flow.
	void NotifyApp(std::string, int);									//Function to send notifications by using the CrtlApp outport.
	void NotifyNeighbors(std::string, std::vector<double> *o, std::vector<double> *f); //Function to inform neighboring APs.
	void NotifySTA(std::string, Notification *n);			//Function to send notifications by using the CrtlSTA outport.
	//void NotifyAgent(AgentNotification &n);					//Function to send notifications by using the CrtlAgent outport.
	void Send(Flow &f);																//Function to send flow by using the DataSTa ouport.
	void CollectStatistics(Flow *f);									//Function to update statistics at the AP.
	void UpdateEFlowLosses(int, double, double);			//Function to update and keep track of flow losses for ealstic flows only.
};

AP::AP (){
	//connect DCA.to_component,DCAByLearning;
}

AP::~AP(){

}

void AP::Setup(){

}

void AP::Start(){
	Initialization();
}

void AP::Stop(){

}

/* ----------------------------------------------------------------------------------
Upon start, the AP is initialized by creating the interfaces and selecting Channel.
The channel selection established a pair of frequency and bandwidth.

- If MLO enabled, the policy is set as the defined by the user. Otherwise, it is set
by the default value of ALL_ONE.
- If Machine learning is enabled, it must contain the agent creation
and configuration.
---------------------------------------------------------------------------------- */

void AP::Initialization(){

	int intSz = maxIntNum;
	for (int i=0; i<intSz; i++){
		//int ChSize = Channels[i].size();
		//std::uniform_int_distribution<int>IntChN(0, ChSize-1);

		APInterface interface;
		interface.id = i;
		//interface.ChN = Channels[i].at(IntChN(gen));
		interface.ChN = Channels[i].at(1);
		std::pair<double, int> p = GetFromChN(interface.ChN);
		interface.fc = p.first;
		interface.ChW = p.second;
		InterfaceContainer.push_back(interface);
	}

	if (capabilities.Multilink){
		policy_manager.setType(policy);
	}
	else{
		policy_manager.setType("ALL_ONE");
	}

	NotifyNeighbors("AP_NEIGHBOR_DISCOVERY", nullptr, nullptr);
}

/* ----------------------------------------------------------------------------------
Function to notify changes between APs:
- AP neighbor discovery -> It triggers the neighbor discovery process. The -1 value,
indicates that the notification can be read by all the deployed APs.
- AP load change -> Inform changes of airtime due to a flow start/end. Also, as each
associated STA may use different interfaces depending on the distance, we pass
the fc vector of the flow wich may start/end.
---------------------------------------------------------------------------------- */

void AP::NotifyNeighbors(std::string type,std::vector<double> *occ, std::vector<double> *fc){

	if (type.compare("AP_NEIGHBOR_DISCOVERY") == 0){
		std::vector<double> int_fc;
		for (int i=0; i<(int)InterfaceContainer.size(); i++){
			int_fc.push_back(InterfaceContainer.at(i).fc);
		}
		Notification notification ("AP_NEIGHBOR_DISCOVERY", apID, -1);
		notification.setPosition(coordinates);
		notification.setCapabilities(capabilities);
		notification.setFc(int_fc);
		outCtrlAP(notification);
	}
	else if ((type.compare("FLOW_START") == 0) || (type.compare("ADD") == 0)){
		for (int i=0; i<(int)NeighboringAPs.size(); i++){
			Notification notification ("AP_LOAD", apID, NeighboringAPs.at(i).id);
			notification.setFlag("ADD");
			notification.setChOcc(*occ);
			notification.setFc(*fc);
			outCtrlAP(notification);
		}
	}
	else if ((type.compare("FLOW_END") == 0) || (type.compare("REMOVE") == 0)){
		for (int i=0; i<(int)NeighboringAPs.size(); i++){
			Notification notification ("AP_LOAD", apID, NeighboringAPs.at(i).id);
			notification.setFlag("REMOVE");
			notification.setChOcc(*occ);
			notification.setFc(*fc);
			outCtrlAP(notification);
		}
	}
}

/* ----------------------------------------------------------------------------------
Function to perform actions advertised by neighboring APs:
- AP neighboring set construction -> constructs the vector of neighboring APs
- AP_LOAD update -> Updates the airtime of neighboring APs and the receiving AP
adds the corresponding values to the neighboring load.
- Channel switch announcement -> Updates the parameters of neighborings APs and,
if overlapping dissapears due to change in fc, update the channel load. Also, it is
checked if an AP still meets the CCA criteria to be included or excluded from
vector of neighboring APs.
---------------------------------------------------------------------------------- */

void AP::inCtrlAP(Notification &n){

	std::string type = n.getType();

	if ((n.getDestination() == -1)){
		if(type.compare("AP_NEIGHBOR_DISCOVERY") == 0){

			Position sender_coordinates = n.getPosition();
			std::vector<double> sender_Fc = n.getFc();
			std::vector<double> overlapping_fc;

			for (int i=0; i<(int)sender_Fc.size(); i++){
				double RxRSSI = CalculateRSSI(configuration.TxPower, sender_Fc.at(i), coordinates.x, coordinates.y, coordinates.z, sender_coordinates.x, sender_coordinates.y, sender_coordinates.z);
				if (RxRSSI >= configuration.CCA){
					overlapping_fc.push_back(sender_Fc.at(i));
				}
			}
			if ((int)overlapping_fc.size() !=0){
				WifiAP ap;
				ap.id = n.getSender();
				ap.coord = n.getPosition();
				ap.ChOcc.assign(overlapping_fc.size(), 0.0);
				for (int i=0; i<(int)overlapping_fc.size(); i++){
					ap.fc.push_back(overlapping_fc.at(i));
				}
				NeighboringAPs.push_back(ap);
			}
		}
	}
	else{
		if (n.getDestination() == apID){
			if (type.compare("AP_LOAD") == 0){
				bool update_load = false;
				for (int i=0; i<(int)NeighboringAPs.size(); i++){
					if (n.getSender() == NeighboringAPs.at(i).id){
						if (NeighboringAPs.at(i).fc.size()!=0){

							std::vector<double> neigh_occ = n.getChOcc();
							std::vector<double> neigh_fc = n.getFc();
							std::string t_flag = n.getFlag();
							for (int j=0; j<(int)NeighboringAPs.at(i).fc.size(); j++){
								for (int n=0; n<(int)neigh_fc.size(); n++){
									if ((NeighboringAPs.at(i).fc.at(j) == neigh_fc.at(n)) && (t_flag.compare("ADD")==0)){
										update_load = true;
										NeighboringAPs.at(i).ChOcc.at(j) += neigh_occ.at(n);
										break;
									}
									else if ((NeighboringAPs.at(i).fc.at(j) == neigh_fc.at(n)) && (t_flag.compare("REMOVE")==0)){
										update_load = true;
										NeighboringAPs.at(i).ChOcc.at(j) -= neigh_occ.at(n);
										if (NeighboringAPs.at(i).ChOcc.at(j) < 1E-10)
											NeighboringAPs.at(i).ChOcc.at(j) = 0;
										break;
									}
								}
							}
						}
					}
				}

				if (update_load){
					for (int i=0; i<(int)InterfaceContainer.size(); i++){
						InterfaceContainer.at(i).TOcc -= InterfaceContainer.at(i).ChOccNeighAPs;
						InterfaceContainer.at(i).ChOccNeighAPs = 0;
						std::string Int_band = GetBand(InterfaceContainer.at(i).fc);
						for (int j=0; j<(int)NeighboringAPs.size(); j++){
							for (int n=0; n<(int)NeighboringAPs.at(j).fc.size(); n++){
								std::string Neigh_band = GetBand(NeighboringAPs.at(j).fc.at(n));
								if (Int_band.compare(Neigh_band) == 0){
									if (CheckNeighChOverlapp(InterfaceContainer.at(i).fc, NeighboringAPs.at(j).fc.at(n))){
										InterfaceContainer.at(i).ChOccNeighAPs += NeighboringAPs.at(j).ChOcc.at(n);
									}
									break;
								}
							}
						}
						InterfaceContainer.at(i).TOcc += InterfaceContainer.at(i).ChOccNeighAPs;
					}
				}
				NotifySTA("SAT_UPDATE", nullptr);
			}
			else if (type.compare("CHANNEL_SWITCH_AP") == 0){
				for (int i=0; i<(int)NeighboringAPs.size(); i++){
					if (n.getSender() == NeighboringAPs.at(i).id){
						NeighboringAPs.at(i).fc.clear(); //Clear previous records to save new selection
						std::vector<double> neigh_fc = n.getFc(); //new set of frequencies
						for (int j=0; j<(int)neigh_fc.size(); j++){
							double RxRSSI = CalculateRSSI(configuration.TxPower, neigh_fc.at(j), coordinates.x, coordinates.y, coordinates.z, NeighboringAPs.at(i).coord.x, NeighboringAPs.at(i).coord.y, NeighboringAPs.at(i).coord.z);
							if (RxRSSI >= configuration.CCA){
								NeighboringAPs.at(i).fc.push_back(neigh_fc.at(j));
							}
						}
						break;
					}
				}
			}
		}
	}
}

/* ----------------------------------------------------------------------------------
Function to create new flows. It is triggered by the start timer on the application
class. The BW selected is expressed in Mbps.
---------------------------------------------------------------------------------- */

Flow AP::CreateFlow(int src, int dest, std::string type, double t){

	Flow flow;
	flow.setSender(src);
	flow.setDestination(dest);
	flow.setType(type);
	flow.setTimeStamp(SimTime());
	flow.setDuration(t);

	if (type.compare("STREAMING")==0){
		std::uniform_int_distribution<int>BWGen(5, 5);
		flow.setLength(BWGen(gen));
	}
	else{
		std::uniform_real_distribution<double>BWGen(2, medBW);
		double B = BWGen(gen);
		for (int i=0; i<(int)AssociatedSTAs.size(); i++){
			if (AssociatedSTAs.at(i).id == dest){
				if (AssociatedSTAs.at(i).EFlowLoss !=0){
					//std::cout << "Elastic losses: " << AssociatedSTAs.at(i).EFlowLoss << " for dest: " << dest << " flow t: " << t << " rand B: " << B;
					B += AssociatedSTAs.at(i).EFlowLoss/(t);
					//std::cout << " new B with losses: " << B <<std::endl;
					flow.setLength(B);
				}
				else{
					flow.setLength(B);
				}
			}
		}
	}

	return flow;
}

/* ----------------------------------------------------------------------------------
Function that registers a new incoming flow into the OngoingFlows vector.
If needed, a queue can be implemented in this function to perform an admission
control mechanism based on the available airtime at the AP.
---------------------------------------------------------------------------------- */

void AP::AcceptIncomingFlow(Flow &f){

	std::string type = f.getType();
	if ((type.compare("STREAMING") == 0)||(type.compare("ELASTIC") == 0)){
		OnGoingFlows.push_back(f);
		RegisterAT(f);
	}
}

/* ----------------------------------------------------------------------------------
Once the flow is registered into the OngoinfFlows vector, the AP must get the
airtime values for each interface and add them to the total airtime being served.
Then, the AP pass the flow to the station, which includes the first satisfaction
values of the flow.
---------------------------------------------------------------------------------- */

void AP::RegisterAT(Flow &f){

	std::vector<double> FTxTimes = f.getTxTime();
	std::vector<double> FlowFc = f.getFc();

	for (int i=0; i<(int)FlowFc.size(); i++){
		for (int j=0; j<(int)InterfaceContainer.size(); j++){
			if (FlowFc.at(i) == InterfaceContainer.at(j).fc){
				InterfaceContainer.at(j).ChOccSFlows += FTxTimes.at(i);
				InterfaceContainer.at(j).TOcc = InterfaceContainer.at(j).ChOccSFlows + InterfaceContainer.at(j).ChOccNeighAPs;
				break;
			}
		}
	}
	Send(f);
	NotifySTA("SAT_UPDATE", nullptr);
	NotifyNeighbors("FLOW_START", &FTxTimes, &FlowFc);
}

/* ----------------------------------------------------------------------------------
Function that performs actions according to the control messages.
- PROBE_RESP -> Probe response to the sender. For discovery purposes.
- MLO_SETUP_RESP -> It performs the MLO setup request according to the link quality.
- SAT_UPDATE -> Announces changes in satisfaction values to stations with alive flows.
- FLOW_END -> Inform that a flow has ended.
---------------------------------------------------------------------------------- */

void AP::NotifySTA(std::string type, Notification *n){

	if (type.compare("PROBE_RESP") == 0){
		Notification notification ("PROBE_RESP", apID, n->getSender());
		notification.setPosition(coordinates);
		notification.setCapabilities(capabilities);
		notification.setFc(InterfaceContainer.at(0).fc);
		outCtrlSTA(notification);
	}
	else if (type.compare("CONFIG_RESP") == 0){
		Notification notification ("CONFIG_RESP", apID, n->getSender());
		notification.setPosition(coordinates);
		for (int i=0; i<(int)InterfaceContainer.size(); i++){
			notification.setFc(InterfaceContainer.at(i).fc);
		}
		outCtrlSTA(notification);
	}
	else if (type.compare("MLO_SETUP_RESP") == 0){
		for (int i=0; i<(int)AssociatedSTAs.size(); i++){
			if (AssociatedSTAs.at(i).id == n->getSender()){

				/* Clear vectors. Its purpose is to reuse the same function when stations perform reassociation with other APs*/
				AssociatedSTAs.at(i).fc.clear();
				AssociatedSTAs.at(i).RSSI.clear();
				AssociatedSTAs.at(i).SNR.clear();
				AssociatedSTAs.at(i).TxRate.clear();

				std::vector<double> sta_linkQ = n->getLinkQuality();
				for (int j=0; j<(int)sta_linkQ.size(); j++){
					if (sta_linkQ.at(j) >= configuration.CCA){
						double Dl_RSSI = CalculateRSSI(configuration.TxPower, InterfaceContainer.at(j).fc, coordinates.x, coordinates.y, coordinates.z, AssociatedSTAs.at(i).coord.x, AssociatedSTAs.at(i).coord.y, AssociatedSTAs.at(i).coord.z);
						double Dl_SNR = CalculateSNR(Dl_RSSI, InterfaceContainer.at(j).ChW);
						double Dl_TxRate = CalculateDataRate(Dl_SNR, InterfaceContainer.at(j).fc, InterfaceContainer.at(j).ChW, capabilities, configuration);

						if (Dl_TxRate != 0){
							AssociatedSTAs.at(i).fc.push_back(InterfaceContainer.at(j).fc);
							AssociatedSTAs.at(i).RSSI.push_back(Dl_RSSI);
							AssociatedSTAs.at(i).SNR.push_back(Dl_SNR);
							AssociatedSTAs.at(i).TxRate.push_back(Dl_TxRate);
						}
					}
				}
				
				//Message to initialize an application for this station.
				NotifyApp("CTRL_START", n->getSender());

				//Send notification
				Notification notification ("MLO_SETUP_RESP", apID, n->getSender());
				notification.setFc(AssociatedSTAs.at(i).fc);
				outCtrlSTA(notification);
			}
		}
	}
	else if (type.compare("SAT_UPDATE") == 0){

		for (int i=0; i<(int)OnGoingFlows.size(); i++){
			std::vector<double> fc = OnGoingFlows.at(i).getFc();
			std::vector<double> txtimes = OnGoingFlows.at(i).getTxTime();
			std::vector<double> ATSat((int)InterfaceContainer.size(), -1.0);

			for (int j=0; j<(int)fc.size(); j++){
				for (int n=0; n<(int)InterfaceContainer.size(); n++){
					//std:: cout << fc.at(j) << " " << InterfaceContainer.at(n).fc << " " << txtimes.at(j) << std::endl;
					if ((fc.at(j) == InterfaceContainer.at(n).fc) && (txtimes.at(j) != 0)){
						double Sat = std::min(1.0,InterfaceContainer.at(n).TOcc)/InterfaceContainer.at(n).TOcc;
						ATSat.at(n) = Sat;
						break;
					}
				}
			}
			OnGoingFlows.at(i).setSat(ATSat);
			Notification notification ("SAT_UPDATE", apID,  OnGoingFlows.at(i).getDestination());
			notification.setSat(ATSat);
			outCtrlSTA(notification);
		}
		CollectStatistics(nullptr);
	}
	else if (type.compare("FLOW_END") == 0){
		outCtrlSTA(*n);
	}
}

/* ----------------------------------------------------------------------------------
Function that manages the control messages sent by stations to the AP.
- PROBE_REQ -> Request essential information for discovery.
- MLO_SETUP_REQ -> Request a MLO setup according to the quality criteria.
- UPDATE_MLO -> In caso of reassociation, perform an update the MLO links.
- STA_DEASSOCIATION -> Deassociate STA from the AP.
---------------------------------------------------------------------------------- */

void AP::NotifyApp(std::string type, int destination){

	if (type.compare("CTRL_START") == 0){
		for (int i=0; i<(int)AssociatedSTAs.size(); i++){
			if (destination == AssociatedSTAs.at(i).id){
				AppCTRL app_ctrl("CTRL_START", apID, destination);
				app_ctrl.setTProfile(AssociatedSTAs.at(i).traffic_type);
				outCtrlApp(app_ctrl);
				break;
			}
		}
	}
	else{
		AppCTRL app_ctrl ("CTRL_CANCEL", apID, destination);
		outCtrlApp(app_ctrl);
	}
}

/* ----------------------------------------------------------------------------------
Function that manages the control messages sent by stations to the AP.
- PROBE_REQ -> Request essential information for discovery.
- MLO_SETUP_REQ -> Request a MLO setup according to the quality criteria.
- UPDATE_MLO -> In case of reassociation, perform an update the MLO links.
- STA_DEASSOCIATION -> Deassociate STA from the AP.
---------------------------------------------------------------------------------- */

void AP::inCtrlSTA(Notification &n){

	std::string type = n.getType();

	if (n.getDestination() == apID){
		if (type.compare("PROBE_REQ") == 0){
			NotifySTA("PROBE_RESP", &n);
		}
		if (type.compare("CONFIG_REQ") == 0){
			NotifySTA("CONFIG_RESP", &n);
		}
		else if(type.compare("MLO_SETUP_REQ") == 0){
			NotifySTA("MLO_SETUP_RESP", &n);
		}
		else if(type.compare("UPDATE_MLO") == 0){
			NotifySTA("MLO_SETUP_RESP", &n);
		}
		else if (type.compare("STA_DEASSOCIATION") == 0){
			for (int i=0; i<(int)AssociatedSTAs.size(); i++){
				if (n.getSender() == AssociatedSTAs.at(i).id){
					AssociatedSTAs.erase(AssociatedSTAs.begin()+i);
					NotifyApp("CTRL_CANCEL", n.getSender());
				}
			}
		}
	}
	else if ((n.getDestination() == -1) && (type.compare("PROBE_REQ") == 0)){
		NotifySTA("PROBE_RESP", &n);
	}
}

/* ----------------------------------------------------------------------------------
This function controls the messages from the application class. Basically, it triggers
the flow creation and allocation through the different interfaces (if MLO is enabled).
Also, when a flow ends, statistics are collected and load updated.
---------------------------------------------------------------------------------- */

void AP::inCtrlApp(AppCTRL &n){

	if (n.getSender() == apID){
		std::string type = n.getType();
		if(type.compare("FLOW_START") == 0){
			for (int i=0; i<(int)AssociatedSTAs.size(); i++){
				if (n.getDestination() == AssociatedSTAs.at(i).id){

					Flow flow = CreateFlow(apID, n.getDestination(), AssociatedSTAs.at(i).traffic_type, n.getDuration());
					std::string policy_type = policy_manager.getType();

					if (policy_type.compare("MSLA") == 0){
						policy_manager.AllocationFromPolicyEqual(&flow, AssociatedSTAs, InterfaceContainer);
					}
					else if (policy_type.compare("SLCI") == 0){
						policy_manager.AllocationFromPolicyOne(&flow, AssociatedSTAs, InterfaceContainer);
					}
					else if (policy_type.compare("MCAA") == 0){
						policy_manager.AllocationFromPolicyFixed(&flow, AssociatedSTAs, InterfaceContainer);
					}
					else if (policy_type.compare("VIDEO_DATA_SPLIT") == 0){
						policy_manager.AllocationFromPolicySplit(&flow, AssociatedSTAs, InterfaceContainer);
					}
					AcceptIncomingFlow(flow);

					break;
				}
			}
		}
		else{
			for (int i=0; i<(int)OnGoingFlows.size(); i++){
				if (n.getDestination() == OnGoingFlows.at(i).getDestination()){

					std::vector<double> FlowFc = OnGoingFlows.at(i).getFc();
					std::vector<double> FTxTimes = OnGoingFlows.at(i).getTxTime();

					for (int j=0; j<(int)FlowFc.size(); j++){
						for (int m=0; m<(int)InterfaceContainer.size(); m++){
							if (FlowFc.at(j) == InterfaceContainer.at(m).fc){
								InterfaceContainer.at(m).ChOccSFlows -= FTxTimes.at(j);
								if (InterfaceContainer.at(m).ChOccSFlows < 1E-10){
			            InterfaceContainer.at(m).ChOccSFlows = 0;
			          }
								InterfaceContainer.at(m).TOcc = InterfaceContainer.at(m).ChOccSFlows + InterfaceContainer.at(m).ChOccNeighAPs;
							}
						}
					}

					Notification notification ("FLOW_END", apID, OnGoingFlows.at(i).getDestination());
					NotifySTA("FLOW_END", &notification);
					CollectStatistics(&OnGoingFlows.at(i));
					OnGoingFlows.erase(OnGoingFlows.begin()+i);

					NotifySTA("SAT_UPDATE", nullptr);
					NotifyNeighbors("FLOW_END", &FTxTimes, &FlowFc);

					break;
				}
			}
		}
	}
}

/* ----------------------------------------------------------------------------------
Function to send flows to associated STAs
---------------------------------------------------------------------------------- */

void AP::Send(Flow &f){
	outDataSTA(f);
}

/* ----------------------------------------------------------------------------------
Function to collect and store statistics regarding transmitted flows. For evaluation
and purposes.
---------------------------------------------------------------------------------- */

void AP::CollectStatistics(Flow *flow){

 if (flow != nullptr){
	 std::vector<double> ChOcc((int)InterfaceContainer.size(), -1), ChRew((int)InterfaceContainer.size(), -1);
	 for (int i=0; i<(int)InterfaceContainer.size(); i++){
		 ChOcc.at(i) = std::min(1.0,InterfaceContainer.at(i).TOcc);
		 ChRew.at(i) = std::max(0.0,1-InterfaceContainer.at(i).TOcc);
	 }
	 statistics.ChOcc.push_back(ChOcc);
	 statistics.ChReward.push_back(ChRew);
	 statistics.SimT.push_back(SimTime());

	 double drop_Ratio = flow->getDratio();
	 std::string flow_type = flow->getType();
	 if (flow_type.compare("ELASTIC") == 0){
		 UpdateEFlowLosses(flow->getDestination(), flow->getLength(), drop_Ratio);
	 }
	 else{
		 statistics.AvgDRPerFlow.push_back(drop_Ratio);
	 }
 }
 else{
	 std::vector<double> ChOcc((int)InterfaceContainer.size(), -1), ChRew((int)InterfaceContainer.size(), -1);
	 for (int i=0; i<(int)InterfaceContainer.size(); i++){
		 ChOcc.at(i) = std::min(1.0,InterfaceContainer.at(i).TOcc);
		 ChRew.at(i) = std::max(0.0,1-InterfaceContainer.at(i).TOcc);
	 }
	 statistics.ChOcc.push_back(ChOcc);
	 statistics.ChReward.push_back(ChRew);
	 statistics.SimT.push_back(SimTime());
	}
}

/* ----------------------------------------------------------------------------------
Function that tracks the flow losses for elastic flows only.
---------------------------------------------------------------------------------- */

void AP::UpdateEFlowLosses(int destination, double length, double dratio){
	for (int i=0; i<(int)AssociatedSTAs.size(); i++){
		if (AssociatedSTAs.at(i).id == destination){
			AssociatedSTAs.at(i).EFlowLoss = length*dratio;
		}
	}
}


/* ----------------------------------------------------------------------------------
To be implemented: Function to perfom machine learning based decisions regarding a
dynamic channel allocation.
---------------------------------------------------------------------------------- */

/*void AP::DCAByLearning(trigger_t&){
}*/

#endif
