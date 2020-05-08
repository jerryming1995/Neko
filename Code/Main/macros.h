//            %%%%%%%%%%%%%%%                                         %%%%%%%%%%%%%%%
//            --------------- Local variables that run the simulation ---------------
//            %%%%%%%%%%%%%%%                                         %%%%%%%%%%%%%%%

// 0- General

const int runTimeSim = 86400;           //Simulation run time, simulate at least 1 Day. Now 5 Days are configured
const int rnd = 1;                      //Generate a random scenairo.
const int lowBW = 2;                    //Low user BW profile. Users that check gmail or use low data traffic.
const int medBW = 5;                    //Med user BW profile. Users that use applications that need certain amount of BW such as Skype (VoIP).
const int highBW = 8;                   //High user BW profile. Users using videostreaming.
const int propagation = 1;              //Propagation model. 0 -> for residential (building) scenario, 1 -> enterprise scenario, 2 -> TMB..
const int LearningWindowTime = 540;     //Window of learning. Currently set to 9 min (540s)
const double ProbUserAgentEnabled = 1;  //Probability of having agent-enabled users (Only when rnd generated scenarios)

// 1- PHY&MAC parameters depending on IEEEprotocol:

// --> For 802.11ax
      const int frameLength = 12000;                    //bits (1500 Bytes)

      const int Lsf = 16;                               //Service field
      const int Lmac = 320;                             //Lenght of MAC header (MAC layer)
      const int Ltb = 18;                               //Tail bits
      const int Lack = 112;                             //Length of an ACK packet
      const int Lrts = 160;                             //Length of a RTS packet
      const int Lcts = 112;                             //Length of a CTS packet
      const int CW = 16;                                //contention window

      const double Tempty = 9*pow(10,-6);               //duration of an empty time slot
      const double Tsifs = 16*pow(10,-6);               //short interframe space
      const double Tdifs = 34*pow(10,-6);               //DCF interframe space
      const double TphyL = 20*pow(10,-6);               //Legacy preamble
      const double TphyHE = 164*pow(10,-6);             //HE single user preamble (PHY layer)
      const double Tofdm_leg = 4*pow(10,-6);            //Symbol duration in legacy mode 3.2usec + 0.8 long GI
      const double Tofdm = 16*pow(10,-6);               //Symbol duration 12.8usec + 3.2usec long GI
      const double legacyRate = 24;                     //bits/symbol --> minimum modulation MCS0,1 or 2

      const double Pe = 0.1;                            //Packet error rate

/* --> For 802.11ac
      const int packetLength = 12000;
      const int Lsf = 16;
      const int Lmac = 240;
      const int Ltb = 6;
      const int Lack = 112;
      const int Lrts = 160;
      const int Lcts = 112;
      const int CW = 15;

      const double Tempty = 9*pow(10,-6);
      const double Tsifs = 16*pow(10,-6);
      const double Tdifs = 34*pow(10,-6);
      const double TphyL = 20*pow(10,-6);
      const double Tphy = 44*pow(10,-6);
      const double Tofdm = 4*pow(10,-6);*/

/* --> For 802.11n

      const int packetLength = 12000;
      const int Lsf = 16;
      const int Lmac = 240;
      const int Ltb = 6;
      const int Lack = 112;
      const int CW = 15;

      const double Tempty = 9*pow(10,-6);
      const double Tsifs = 16*pow(10,-6);
      const double Tdifs = 34*pow(10,-6);
      const double TphyL = 20*pow(10,-6);
      const double Tphy = 44*pow(10,-6);
      const double Tofdm = 4*pow(10,-6);*/

// 2- Learning mechanisms

const int stationLearningFlag = 2;      //ML flag for stations: 0-> No learning, 1-> eGreedy algorithm, 2-> Thompson sampling, 3...
const int APlearningFlag = 2;           //ML flag for APs: 0-> No learning, 1-> eGreedy algorithm, 2-> Thompson sampling, 3...


// 3- Flow control

const double flowDuration = 20;            //Flag to set the duration of a flow. It follows an exponential distribution with mean = value // Exp(value)
const double flowActivation = 60;          //Flag to set the time that a station becomes active. It follows an exponential distribution with mean = value // Exp(value)

// 4- Timers and variables used to control activation of functions

const int SearchBestAP = 180;           //Timer to activate AP selection at the stations.
const int ChSelectionTime = 180;        //Timer to activate CH selection at the AP.

const int offsetAP = 7200;              //Time that the system remains static for APs
const int offsetSTA = 7200;             //Time that the system remains static for Stations
