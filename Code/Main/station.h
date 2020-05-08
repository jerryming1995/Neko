#ifndef _STA_
#define _STA_


#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <time.h>
#include <deque>


component Station : public TypeII {

public:
  int staID;                                                        //Client iD (starting at 0).
  int userType;                                                     //Flag to control user types (High (0), Med (1) or Low (2) traffic)
  int servingAP;                                                    //Id of the serving AP.
  double X, Y, Z;                                                   //Coordinates of the client.

  int txPower;                                                      //TxPower of each client.
  double DLDataRate;                                                //Downlink data rate.
  double ULDataRate;                                                //Uplink data rate.
  double RSSI;                                                      //RSSI value received at the client.
  int ChBW;                                                         //Channel BW used.
  double frequency;                                                 //TX frequency.
  int IEEE_protocol;                                                //IEEE Protocol.

  double AirTimeRequired;                                           //Air time required by station.
  int FlowType;                                                     //Flow type -> 0 DL flow, 1 UL flow.

  std::deque <APBeacon> detectedWLANs;                              //Beacons detected from each WLAN.
  std::vector<int> InRangeAPs;                                      //Id of detected APs within a CCA range
  std::vector<int> CandidateAPs;                                    //Id of elegible APs to perform client steering


  //Traces for evaluation purposes.
  std::vector<double> AirTimeEvo;                                   //Vector to store airtime.
  std::vector<double> Satisfaction;                                 //Vector to store satisfaction.
  std::vector<double> CandidateAPsTrafficLoad;                      //Vector to store the last traffic load registered from a certain AP.

  std::vector<double> reward_action;                                //Vector to store avg. reward whitin a window (30 min).
  std::vector<double> estimated_reward_action;
  std::vector<double> occupancy_AP;                                 //Vector to store avg. occupancy detected of an AP within a window.

  std::vector<int> Times_ActionSelected;                            //Times arm has been selected.
  std::vector<int> Action_Selected;                                 //Vector to store the APs selected when eGreedy.
  std::vector<int> ActionChange;
  std::vector<double> TimeStamp;
  std::vector<double> Throughput;

  std::vector< std::vector<double> > estimated_reward_Per_action;    //Matrix to store estimated reward per action;
  std::vector< std::vector<double> > estimated_reward_Per_action_Time;

  std::vector<double> mSat;
  std::vector<double> timeSim;                                      //Time stamp of simulation for the airtime value.
  std::vector<double> timeSim2;                                     //Time stamp of simulation for the satisfaction value.

  //Global scope variables.
  double iter;
  int flag, TimeSizeF, TimeSizeS;
  double requestedBW, startTX, finishTX, bits_Sent, totalBits, timeActive;

public:

  Station();

  // COST
  void Setup();
  void Start();
  void Stop();

  //Inports
  inport void inReceivedBeacon(APBeacon &b);
  inport void inAssignedAirTime(Connection &n);
  inport void inUpdateStationParameters(APBeacon &b);

  //Outports
  outport void outSetClientAssociation(StationInfo &i);
  outport void outRequestAirTime(Connection &n);
  outport void outFlowEnded(Connection &n);
  outport void outUpdateAttachedStationsParams(StationInfo &i);
  outport void outUpdateConnection(StationInfo &i, int k);

  //Triggers
  Timer <trigger_t> trigger_ProcessBeacons;
  Timer <trigger_t> trigger_SendRequestedAT;
  Timer <trigger_t> trigger_TxTimeFinished;
  Timer <trigger_t> trigger_Action;

  inport inline void ProcessBeacons(trigger_t&);
  inport inline void SendRequestedAT(trigger_t&);
  inport inline void SendTxTimeFinished(trigger_t&);
  inport inline void APselectionBylearning(trigger_t&);

};

Station :: Station(){
  connect trigger_ProcessBeacons.to_component,ProcessBeacons;
  connect trigger_SendRequestedAT.to_component,SendRequestedAT;
  connect trigger_TxTimeFinished.to_component,SendTxTimeFinished;
  connect trigger_Action.to_component,APselectionBylearning;
}

void Station :: Setup(){

}

void Station :: Start(){
  iter = 1;
  flag = 0;
  TimeSizeF = 0;
  TimeSizeS = 0;
}

void Station :: Stop(){

  int TimeWrap = 900;
  double pos = runTimeSim/TimeWrap;
  int step = 0;
  int count = 0;
  int endVal = TimeWrap-1;
  double tmp_mean = 0;
  double m = 0;

  for (int k=0; k<pos; k++){
    for (int init=0; init<(int)timeSim2.size(); init++){
      if (step <= timeSim2.at(init) && timeSim2.at(init) < endVal){
        tmp_mean = tmp_mean + Satisfaction.at(init);
        count = count + 1;
      }
    }
    m = tmp_mean/count;

    mSat.push_back(m);

    tmp_mean = 0;
    m = 0;
    count = 0;
    step = step+TimeWrap;
    endVal = endVal+TimeWrap;
  }
}

void Station :: inReceivedBeacon(APBeacon &b){

  detectedWLANs.push_back(b);
  trigger_ProcessBeacons.Set(SimTime()+0.001);
}

void Station :: ProcessBeacons(trigger_t&){

  StationInfo info;

  std::vector<double>RSSIvalueUL;
  std::vector<double>DLDataRates;
  std::vector<double>ULDataRates;
  std::vector<double>InRangeAPsRSSI;

  if (detectedWLANs.size() !=0){

    while (detectedWLANs.size() > 0) {
      double tmpRSSIvalueDL, tmpRSSIvalueUL, DL_r, UL_r;

      APBeacon b = detectedWLANs.front();
      tmpRSSIvalueUL = txPower - PropL(X, Y, Z, b.header.X, b.header.Y, b.header.Z, b.freq);

      if (tmpRSSIvalueUL> -80){

        tmpRSSIvalueDL = b.Tx_Power - PropL(X, Y, Z, b.header.X, b.header.Y, b.header.Z, b.freq);

        InRangeAPs.push_back(b.header.sourceID);
        InRangeAPsRSSI.push_back(tmpRSSIvalueDL);
        RSSIvalueUL.push_back(tmpRSSIvalueUL);

        DL_r = SetDataRate(tmpRSSIvalueDL, b.protocolType, b.BW);
        UL_r = SetDataRate(tmpRSSIvalueUL, b.protocolType, b.BW);

        DLDataRates.push_back(DL_r);
        ULDataRates.push_back(UL_r);

        if (tmpRSSIvalueUL >= -75)
        {
          CandidateAPs.push_back(b.header.sourceID);
        }
      }
      detectedWLANs.pop_front();
    }
  }

  switch (stationLearningFlag) {
    case 0:{ //No learning, best RSSI
        double RSSI_UL;

        RSSI = -100;
        servingAP = 0;

        for (int i=0; i<(int)InRangeAPsRSSI.size(); i++){
          if (RSSI<=InRangeAPsRSSI.at(i)){
            servingAP = InRangeAPs.at(i);
            RSSI = InRangeAPsRSSI.at(i);
            RSSI_UL = RSSIvalueUL.at(i);
            DLDataRate = DLDataRates.at(i);
            ULDataRate = ULDataRates.at(i);
          }
        }

        info.header.sourceID = staID;
        info.header.destinationID = servingAP;
        info.RSSI = RSSI_UL;

        outSetClientAssociation(info);
        trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));

      }break;

    case 1:{ //Epsilon Greedy algorithm, first AP selection is done by best RSSI

        double RSSI_UL;
        int size = (int)CandidateAPs.size();

        RSSI = -100;
        servingAP = 0;

        reward_action.assign(size,0.0);
        occupancy_AP.assign(size,0.0);
        Times_ActionSelected.assign(size,0.0);

        for (int i=0; i<(int)InRangeAPsRSSI.size(); i++){
          if (RSSI<=InRangeAPsRSSI.at(i)){
            servingAP = InRangeAPs.at(i);
            RSSI = InRangeAPsRSSI.at(i);
            RSSI_UL = RSSIvalueUL.at(i);
            DLDataRate = DLDataRates.at(i);
            ULDataRate = ULDataRates.at(i);
          }
        }

        info.header.sourceID = staID;
        info.header.destinationID = servingAP;
        info.RSSI = RSSI_UL;
        outSetClientAssociation(info);

        if (size > 1){
          trigger_Action.Set(SimTime()+offsetSTA);
        }
        trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));

    }break;

    case 2:{ //Thompson Sampling, first AP selection is done by best RSSI

        double RSSI_UL;
        int size = (int)CandidateAPs.size();

        RSSI = -100;
        servingAP = 0;

        reward_action.assign(size,0.0);
        occupancy_AP.assign(size,0.0);
        estimated_reward_action.assign(size,0.0);
        Times_ActionSelected.assign(size,0.0);
        estimated_reward_Per_action.resize(size);
        estimated_reward_Per_action_Time.resize(size);

        for (int i=0; i<(int)InRangeAPsRSSI.size(); i++){
          if (RSSI<=InRangeAPsRSSI.at(i)){
            servingAP = InRangeAPs.at(i);
            RSSI = InRangeAPsRSSI.at(i);
            RSSI_UL = RSSIvalueUL.at(i);
            DLDataRate = DLDataRates.at(i);
            ULDataRate = ULDataRates.at(i);
          }
        }

        info.header.sourceID = staID;
        info.header.destinationID = servingAP;
        info.RSSI = RSSI_UL;
        outSetClientAssociation(info);

        if ((size > 1)&&(userType == 2)){
          trigger_Action.Set(SimTime()+offsetSTA);
        }

        trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));

    }break;
  }

  ActionChange.push_back(servingAP);
  TimeStamp.push_back(SimTime());

}

void Station :: SendRequestedAT(trigger_t&){

  double TimeMPDU, Tack, Trts, Tcts, LDBPS_DL, LDBPS_UL;

  Connection ConnRequest;

  LDBPS_DL = (DLDataRate*pow(10,6))*Tofdm;
  LDBPS_UL = (ULDataRate*pow(10,6))*Tofdm;

  FlowType = 0; //To avoid collisions

  requestedBW = Random(medBW) + 1;

  switch (FlowType) {
    case 0:{ //DL flow

      TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+frameLength+Ltb)/(LDBPS_DL)))*Tofdm;
      Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
      Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
      Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

      AirTimeRequired = (std::ceil((requestedBW*pow(10,6)/frameLength))*(1/(1-Pe)))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty));

      ConnRequest.header.sourceID = staID;
      ConnRequest.header.destinationID = servingAP;
      ConnRequest.LoadByStation = AirTimeRequired*100;
      break;
    }
    case 1:{ //UL flow
      TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+frameLength+Ltb)/(LDBPS_UL)))*Tofdm;
      Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
      Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
      Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

      AirTimeRequired = (std::ceil((requestedBW*pow(10,6)/frameLength))*(1/(1-Pe)))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty));

      ConnRequest.header.sourceID = staID;
      ConnRequest.header.destinationID = servingAP;
      ConnRequest.LoadByStation = AirTimeRequired*100;
      break;
    }
  }

  startTX = SimTime();
  TimeSizeS = (int)timeSim2.size();

  outRequestAirTime(ConnRequest);

  AirTimeEvo.push_back(AirTimeRequired);
  timeSim.push_back(SimTime());

  trigger_TxTimeFinished.Set(SimTime()+Exponential(flowDuration));
}

void Station :: inAssignedAirTime(Connection &response){

  if (staID == response.header.destinationID){
    double MaxLoad;

    if (0 < AirTimeRequired){
      MaxLoad = 100;
      if (response.Ap_Load < 100){
        CandidateAPsTrafficLoad.push_back(response.Ap_Load);
        Satisfaction.push_back((double)1);
      }
      else{
        CandidateAPsTrafficLoad.push_back((double)100);
        Satisfaction.push_back(((std::min(MaxLoad,response.Ap_Load))/(response.Ap_Load)));
      }
      Action_Selected.push_back(servingAP);
      timeSim2.push_back(SimTime());
    }
  }
}

void Station :: SendTxTimeFinished(trigger_t&){

  finishTX = SimTime();
  TimeSizeF = (int)timeSim2.size();
  int size = TimeSizeF-TimeSizeS;
  double throughput = GetData(FlowType, TimeSizeS, size, &Satisfaction, requestedBW);

  Throughput.push_back(throughput);
  bits_Sent = bits_Sent + (GetData(FlowType, TimeSizeS, size, &Satisfaction, requestedBW)*(finishTX-startTX));
  totalBits = totalBits + (requestedBW*(finishTX-startTX));
  //timeActive = timeActive + (finishTX-startTX);

  Connection ConnFinish;
  ConnFinish.header.destinationID = servingAP;
  ConnFinish.header.sourceID = staID;
  ConnFinish.LoadByStation = AirTimeRequired*100;

  outFlowEnded(ConnFinish);

  AirTimeRequired = 0;
  AirTimeEvo.push_back(AirTimeRequired);
  timeSim.push_back(SimTime());

  trigger_SendRequestedAT.Set(SimTime()+Exponential(flowActivation));
}

void Station :: inUpdateStationParameters(APBeacon &b){

  if (staID == b.header.destinationID){
    double RSSIvalueUL;
    StationInfo info;

    RSSI = b.Tx_Power - PropL(X,Y,Z,b.header.X,b.header.Y,b.header.Z,b.freq);
    RSSIvalueUL = txPower - PropL(X,Y,Z,b.header.X,b.header.Y,b.header.Z,b.freq);
    frequency = b.freq;
    IEEE_protocol = b.protocolType;
    ChBW = b.BW;

    DLDataRate = SetDataRate(RSSI, IEEE_protocol, ChBW);
    ULDataRate = SetDataRate(RSSIvalueUL, IEEE_protocol, ChBW);

    info.header.sourceID = staID;
    info.header.destinationID = b.header.sourceID;
    info.RSSI = RSSI;

    outUpdateAttachedStationsParams(info);
  }
}

void Station :: APselectionBylearning(trigger_t&){

  if (trigger_TxTimeFinished.Active() == 0){

    int oldAP = servingAP;
    int num_arms = CandidateAPs.size();
    int index = SearchAction(servingAP, num_arms, &CandidateAPs);

    switch (stationLearningFlag) {

      case 1:{  /*Epsilon Greedy strategy */

        if (flag == 0){
          int i = rand()%num_arms;
          servingAP = CandidateAPs.at(i);
          Times_ActionSelected[i] = Times_ActionSelected[i] + 1;

          flag++;
        }
        else{
          double epsilon = 1/sqrt(iter);
          reward_action[index] = GetReward(servingAP, &Satisfaction, &Action_Selected, &timeSim2, SimTime());
          occupancy_AP[index] = GetOccupancy(servingAP, &CandidateAPsTrafficLoad, &Action_Selected, &timeSim2, SimTime());
          servingAP = CandidateAPs.at(Egreedy(num_arms, &reward_action, &occupancy_AP, epsilon, &Times_ActionSelected));
        }

        StationInfo hello;
        hello.header.sourceID = staID;
        hello.header.destinationID = servingAP;
        hello.RSSI = 0;

        if (oldAP != servingAP){
          outUpdateConnection(hello, oldAP);
        }

        ActionChange.push_back(servingAP);
        TimeStamp.push_back(SimTime());

        trigger_Action.Set(SimTime()+SearchBestAP);
        iter++;

      }break;

      case 2:{  /*Thompson Sampling strategy */

        if (flag == 0){
          estimated_reward_action[index] = ((estimated_reward_action[index] * Times_ActionSelected[index]) + (reward_action[index]))/ (Times_ActionSelected[index] + 2);
          estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
          estimated_reward_Per_action_Time[index].push_back(SimTime());
          servingAP = CandidateAPs.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_AP, &Times_ActionSelected));
          flag++;
        }
        else{
          reward_action[index] = GetReward(servingAP, &Satisfaction, &Action_Selected, &timeSim2, SimTime());
          occupancy_AP[index] = (GetOccupancy(servingAP, &CandidateAPsTrafficLoad, &Action_Selected, &timeSim2, SimTime()))/100;
          estimated_reward_action[index] = ((estimated_reward_action[index] * Times_ActionSelected[index]) + (reward_action[index]))/ (Times_ActionSelected[index] + 2);
          estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
          estimated_reward_Per_action_Time[index].push_back(SimTime());
          servingAP = CandidateAPs.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_AP, &Times_ActionSelected));
        }

        StationInfo hello;
        hello.header.sourceID = staID;
        hello.header.destinationID = servingAP;
        hello.RSSI = 0;

        if (oldAP != servingAP){
          outUpdateConnection(hello, oldAP);
        }

        ActionChange.push_back(servingAP);
        TimeStamp.push_back(SimTime());

        trigger_Action.Set(SimTime()+SearchBestAP);
      }break;
    }
  }

  else{
    double t = trigger_TxTimeFinished.GetTime() - SimTime() + 0.001;
    trigger_Action.Cancel();
    trigger_Action.Set(SimTime()+t);
  }
}

#endif
