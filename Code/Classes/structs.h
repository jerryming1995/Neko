
// --------------------------------------------------------------------------------------------------------------------------------------------------//

/* General purpose structures */

struct Position{

	double x;										//x coordinate (m)
	double y;										//y coordinate (m)
	double z;										//z coordinate (m)
};

struct Capabilities{

	int IEEEProtocol;						//IEEE 802.11 protocol version (0.ac, 1.ax, 2.be)
	bool Multilink;							//Whether a node is multilink capable or not (true, false)
	bool Mlearning;							//Whether a node is agent enabled or not (true, false)
};

struct Configuration{

	int nSS;										//number of spatial streams.
	double TxPower;							//Tx power configured
	double CCA;									//CCA Threshold
};

// --------------------------------------------------------------------------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------------------------------------------------------------------------//

/* Data base like structures */

struct WifiSTA{

	int id;												//Identifier of the STA
	double EFlowLoss;							//Vector that tracks the losses of the flow, per station, and adds them at the beggining of a new flow. Thus, EFlows must register 0 losses.
	Position coord;								//Position of the STA
	std::string traffic_type;			//Type of traffic for the STA
	std::vector<double> fc;				//vector of frenquencies in which an station is allowed to operate.
	std::vector<double> RSSI;			//Vector of RSSIs for each band in which a station operates. DL view.
	std::vector<double> SNR;			//Vector of SNRs for each band in which a station operates. DL view.
	std::vector<double> TxRate;		//Vector of dataRates foer each band in which a station operates. DL view.
};

struct WifiAP{

	int id;												//Identifier of the AP.
	Position coord;								//Position of the AP
	std::vector<double> fc;				//vector of frenquencies in which an AP operates.
	std::vector<double> ChOcc;		//Channel occupancy due to ongoing flows at the AP.
};

// --------------------------------------------------------------------------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------------------------------------------------------------------------//

/* Containers to store the results of the simulation */

struct APStatistics{

	std::vector<std::vector<int>> ChSelection;			//Channel selection performed during the simulation. Rows are number of entries, and columns are number of active interfaces
	std::vector<std::vector<double>> ChReward;			//Channel reward experienced. Rows are number of entries, and columns are number of active interfaces
	std::vector<std::vector<double>> ChOcc;					//Channel occupancy registered. Rows are number of entries, and columns are number of active interfaces
	std::vector<double> SimT;												//Simulation time that follows the channel occupancy for each interface.
	std::vector<double> AvgDRPerFlow;
};

struct STAStatistics{

	std::vector<int> APSelection;										//Serving AP selection
	std::vector<std::vector<double>> SatEvo;				//Satisfaction evolution registered for each interface.
	std::vector<std::vector<double>> SimT;					//Simulation Time that follows the channel occupancy for each interface
	std::vector<double> AvgSatPerFlow;
	std::vector<double> AvgThPerFlow;
	std::vector<double> AvgIdealThPerFlow;
};
