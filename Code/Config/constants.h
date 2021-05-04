//            %%%%%%%%%%%%%%%                                         %%%%%%%%%%%%%%%
//            --------------- Local variables that run the simulation ---------------
//            %%%%%%%%%%%%%%%                                         %%%%%%%%%%%%%%%

// 0- General

const int runTime = 120;           	//Simulation run time, simulate at least 1 Day. Now 5 Days are configured
const int rnd = 1;                      //Generate a random scenairo.
const int lowBW = 2;                    //Low user BW profile. Users that check gmail or use low data traffic.
double medBW;                    //Med user BW profile. Users that use applications that need certain amount of BW such as Skype (VoIP).
const int highBW = 8;                   //High user BW profile. Users using videostreaming.
const int propagation = 1;              //Propagation model. 0 -> for residential (building) scenario, 1 -> enterprise scenario, 2 -> TMB..
const int WinTime = 540;     			//Window of learning. Currently set to 9 min (540s)
const double RSSIth = -75;				//Threshold to avoid APs with low SNR levels.
const int maxIntNum = 3;
bool channel_report = false;				//To generate channel occupancy report
bool stats_report = true;					//To generate statistics report
//int numSTA;

//std::string policy = "PROP_FIXED";
std::string policy;

// 1-Available frequency bands

/*const std::vector<std::vector<double>> Channels{{1,6,11},
																								{38,46,42},
																								{55,71,47}};*/

const std::vector<std::vector<double>> Channels{{1,6,11},
																								{38,46,58},
																								{55,71,15}};

/*const std::map<int,std::pair<double,int>> ChMap{{1, std::make_pair(2.412, 20)},{6, std::make_pair(2.437, 20)},{11, std::make_pair(2.462, 20)}, {3, std::make_pair(2.422, 40)},
																								{36, std::make_pair(5.18, 20)},{38, std::make_pair(5.19, 40)},{46, std::make_pair(5.23, 40)},{42, std::make_pair(5.21, 80)},
																								{51, std::make_pair(6.195, 40)},{55, std::make_pair(6.215, 80)},{71, std::make_pair(6.295, 80)},{47, std::make_pair(6.175, 160)}};*/

// 2- PHY&MAC parameters depending on IEEEprotocol:

const int Lpckt = 12000;                          //bits (1500 Bytes)
const int Lsf = 16;                               //Service field
const int Lmac = 320;                             //Lenght of MAC header (MAC layer)
const int Ltb = 18;                               //Tail bits
//const int Lmd = 32;															//MPDU Delimiter
const int Lack = 112;															//Length of an ACK packet
//const int Lback = 432;                            //Length of an BACK packet
const int Lrts = 160;                             //Length of a RTS packet
const int Lcts = 112;                             //Length of a CTS packet
const int CW = 15;                                //contention window
const double Tempty = 9*pow(10,-6);               //duration of an empty time slot
const double Tsifs = 16*pow(10,-6);               //short interframe space
const double Tdifs = 34*pow(10,-6);               //DCF interframe space
const double TphyL = 20*pow(10,-6);               //Legacy preamble
const double TphyHE = 164*pow(10,-6);             //HE single user preamble (PHY layer)
const double Tofdm_leg = 4*pow(10,-6);            //Symbol duration in legacy mode 3.2usec + 0.8 long GI
const double Tofdm = 16*pow(10,-6);               //Symbol duration 12.8usec + 3.2usec long GI
const double legacyRate = 24;                     //bits/symbol --> minimum modulation MCS0,1 or 2. Here configured as MCS 0, which means 6Mbps
const double Pe = 0.1;                            //Packet error rate
const int NF = 7;								  								//Noise figure
//const int Na = 64;
// 3- Learning mechanisms

//const int stationLearningFlag = 2;      //ML flag for stations: 0-> No learning, 1-> eGreedy algorithm, 2-> Thompson sampling, 3...
//const int APlearningFlag = 2;           //ML flag for APs: 0-> No learning, 1-> eGreedy algorithm, 2-> Thompson sampling, 3...

// 4- Flow control

const double t_EndFlow = 1;            //Flag to set the duration of a flow. It follows an exponential distribution with mean = value // Exp(value)
const double t_ActFlow = 3;            //Flag to set the time that a station becomes active. It follows an exponential distribution with mean = value // Exp(value)

// 5- Timers and variables used to control activation of functions

//const int DAPS_period = 180;           //Timer to activate AP selection at the stations.
//const int DCA_period = 180;        	 //Timer to activate CH selection at the AP.

const int off_DCA = 50;              //Time that the system remains static for APs
const int off_DAPS = 7200;             //Time that the system remains static for Stations
