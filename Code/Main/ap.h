#ifndef _AP_
#define _AP_

#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <vector>
#include <string>

component AP : public TypeII {

public:

  //General parameters
  int apID;                                                     //AP iD (starting at 0).
  double X, Y, Z;                                               //AP or Relay Coordinates.
  int numOfConnectedStations;                                   //Number of connected stations to a certain AP.
  double trafficLoad;                                           //Instantaneous traffic load of the device.

  int actionSelected;
  int channelBW;                                                //Channel BW used to TX data.
  int txPower;                                                  //Tx power configured in the APs.
  int OperatingChannel;                                         //Operating WiFi channel. Frequency will depend on this parameter.
  double frequency;                                             //Central channel frequency.
  double CCA;                                                   //Clear Channel Assessment (CCA) threshold level.

  int IEEEprotocolType;                                         //Flag to indicate the IEEE protocol used. 0 --> 802.11n, 1 --> 802.11ac, 2 --> 802.11ax.
  std::vector<int> CHMapToAction;                               //Available Channels to map with actions.
  std::vector<int> setOfactions;                                //Set of actions that an AP can take.

  //Track users and neighbors
  std::vector<int> vectorOfConnectedStations;                   //List of attached clients to the AP.
  std::vector<double> vectorOfConnectedStationsRSSIs;           //List of clients' RSSI.

  std::vector<int> vectorOfNeighboringAPs;                      //List of neighboring APs/Relays.
  std::vector<double> vectorOfNeighboringRSSIs;                 //List of neighboring APs' RSSI.

  std::vector<double> CH_occupancy_detected;                    //List that tracks the loads in other channels. It is used when an AP is moved to other channel.

  //Learning
  std::vector<double> reward_action;                            //Vector to store avg. reward whitin a window (30 min).
  std::vector<double> occupancy_CH;                             //Vector to store avg. occupancy detected of an AP within a window.
  std::vector<double> estimated_reward_action;
  std::vector<int> TimesActionIsPicked;                               //Times an action is selected.
  std::vector< std::vector<double> > estimated_reward_Per_action;    //Matrix to store estimated reward per action;
  std::vector< std::vector<double> > estimated_reward_Per_action_Time;

  //Vectors to store values for later processing
  std::vector<int> Action_Selected;                             //Vector to store the curren action. A new entry is registered at the start and at the end of a connection.
  std::vector<double> channel_reward;                           //Channel reward experienced. A new entry is registered at the start and at the end of a connection.
  std::vector<double> occupanyOfAp;                             //Occupancy experienced. A new entry is registered at the start and at the end of a connection.
  std::vector<double> TimeSimulation;

  std::vector<int> ActionChange;
  std::vector<double> TimeStamp;

  double iter;
  int flag, isolation;

public:

  AP();

  // COST
  void Setup();
  void Start();
  void Stop();

  //Inports
  inport void inSetNeighbors(APBeacon &b);
  inport void inSetClientAssociation(StationInfo &i);
  inport void inRequestedAirTime(Connection &n);
  inport void inTxTimeFinished(Connection &n);
  inport void inLoadFromNeighbor(ApNotification &ap);
  inport void inUpdateConnection(StationInfo &i, int k);
  inport void inUpdateAttachedStationsParams(StationInfo &i);

  //Outports
  outport void outSetNeighbors(APBeacon &b);
  outport void outSendBeaconToNodes(APBeacon &b);
  outport void outAssignAirTime(Connection &n);
  outport void outLoadToNeighbor(ApNotification &ap);
  outport void outChannelChange(APBeacon &b);

  //Triggers
  Timer <trigger_t> trigger_Action;
  Timer <trigger_t> trigger_APBootUp;
  Timer <trigger_t> trigger_progress;

  inport inline void CHselectionBylearning(trigger_t&);
  inport inline void APBootUp(trigger_t&);
  inport inline void ProgressFunct(trigger_t&);
};

AP :: AP(){
  connect trigger_Action.to_component,CHselectionBylearning;
  connect trigger_APBootUp.to_component,APBootUp;
  connect trigger_progress.to_component,ProgressFunct;
}

void AP :: Setup(){

}

void AP :: Start(){

  switch (IEEEprotocolType) {
    case 0:{
      int actions [] = {0,1,2};
      int Channel [] = {1,6,11};
      int size = sizeof(actions)/sizeof(int);

      for (int i=0; i<size; i++){
        setOfactions.push_back(actions[i]);
        CHMapToAction.push_back(Channel[i]);
      }
      TimesActionIsPicked.assign(size,0.0);
      CH_occupancy_detected.assign(size, 0.0);
      trigger_APBootUp.Set(0);

    }break;

    case 1:{

      int actions [] = {0,1,2,3};
      int Channel [] = {36,40,44,48};
      int size = sizeof(actions)/sizeof(int);

      for (int i=0; i<size; i++){
        setOfactions.push_back(actions[i]);
        CHMapToAction.push_back(Channel[i]);
      }
      TimesActionIsPicked.assign(size,0.0);
      CH_occupancy_detected.assign(size, 0.0);
      trigger_APBootUp.Set(0);

    }break;

    case 2:{

      int actions [] = {0,1,2};
      int Channel [] = {36,40,44};
      int size = sizeof(actions)/sizeof(int);

      for (int i=0; i<size; i++){
        setOfactions.push_back(actions[i]);
        CHMapToAction.push_back(Channel[i]);
      }
      TimesActionIsPicked.assign(size,0.0);
      CH_occupancy_detected.assign(size, 0.0);

      trigger_APBootUp.Set(0);

    }break;
  }

  if (apID == 0) {
    trigger_progress.Set(4320);
  }

  iter = 1;
  flag = 0;
}

void AP :: Stop(){

}

void AP :: APBootUp(trigger_t&){

  double *selectConfiguration = GetConfiguration(IEEEprotocolType, actionSelected);

  channelBW = *selectConfiguration;
  OperatingChannel = *(selectConfiguration+1);
  frequency = *(selectConfiguration+2);

  APBeacon beacon;

  beacon.header.sourceID = apID;
  beacon.header.X = X;
  beacon.header.Y = Y;
  beacon.header.Z = Z;
  beacon.Tx_Power = txPower;
  beacon.freq = frequency;
  beacon.protocolType = IEEEprotocolType;
  beacon.BW = channelBW;

  if (isolation == 0) {
    outSetNeighbors(beacon);
  }

  outSendBeaconToNodes(beacon);

  ActionChange.push_back(actionSelected);
  TimeStamp.push_back(SimTime());

  switch (APlearningFlag) {
    case 0:{
      // do nothing
    }break;

    case 1:{

      int size = setOfactions.size();

      reward_action.assign(size,0.0);
      occupancy_CH.assign(size,0.0);

      trigger_Action.Set(SimTime()+Exponential(2)+offsetAP);

    }break;

    case 2:{

      int size = setOfactions.size();

      reward_action.assign(size,0.0);
      occupancy_CH.assign(size,0.0);
      estimated_reward_action.assign(size,0.0);
      estimated_reward_Per_action.resize(size);
      estimated_reward_Per_action_Time.resize(size);

      trigger_Action.Set(SimTime()+Exponential(2)+offsetAP);

    }break;
  }
}

void AP :: inSetNeighbors (APBeacon &b){

  double RSSIvalue;
  RSSIvalue = txPower - PropL(b.header.X,b.header.Y,b.header.Z,X,Y,Z,b.freq);

  if (RSSIvalue>=CCA){
    vectorOfNeighboringAPs.push_back(b.header.sourceID);
    vectorOfNeighboringRSSIs.push_back(RSSIvalue);
  }
}

void AP :: inSetClientAssociation (StationInfo &s){

  if (s.header.destinationID == apID){

    numOfConnectedStations++;
    vectorOfConnectedStations.push_back(s.header.sourceID);
    vectorOfConnectedStationsRSSIs.push_back(s.RSSI);
  }
}

void AP :: inRequestedAirTime(Connection &STARequest){

  if (apID == STARequest.header.destinationID){
    trafficLoad = trafficLoad + STARequest.LoadByStation;

    Connection APResponse;
    APResponse.header.sourceID = apID;
    APResponse.Ap_Load = trafficLoad;

    if ((int)vectorOfConnectedStations.size() != 0){
      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        APResponse.header.destinationID = vectorOfConnectedStations.at(i);
        outAssignAirTime(APResponse);
      }
    }

    ApNotification AddLoad;
    AddLoad.Load = STARequest.LoadByStation;
    AddLoad.ChannelNumber = OperatingChannel;
    AddLoad.flag = 1;

    if ((int)vectorOfNeighboringAPs.size() != 0){
      for (int i=0;i<(int)vectorOfNeighboringAPs.size();i++){
        AddLoad.header.destinationID = vectorOfNeighboringAPs.at(i);
        outLoadToNeighbor(AddLoad);
      }
    }

    channel_reward.push_back(std::max(0.0,1-(double)(trafficLoad/100)));
    Action_Selected.push_back(actionSelected);

    if (trafficLoad < 100){
      occupanyOfAp.push_back(trafficLoad);
      TimeSimulation.push_back(SimTime());
    }
    else{
      occupanyOfAp.push_back((double)100);
      TimeSimulation.push_back(SimTime());
    }
  }
}

void AP :: inTxTimeFinished(Connection &EndConn){

  if (apID == EndConn.header.destinationID){
    trafficLoad = trafficLoad - EndConn.LoadByStation;

    if (trafficLoad < 0.00001){
      trafficLoad = 0;
    }

    Connection APResponse;
    APResponse.Ap_Load = trafficLoad;
    APResponse.header.sourceID = apID;

    if ((int)vectorOfConnectedStations.size() != 0){
      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        if (vectorOfConnectedStations.at(i) !=EndConn.header.sourceID){
          APResponse.header.destinationID = vectorOfConnectedStations.at(i);
          outAssignAirTime(APResponse);
        }
      }
    }

    ApNotification RemoveLoad;
    RemoveLoad.Load = EndConn.LoadByStation;
    RemoveLoad.ChannelNumber = OperatingChannel;
    RemoveLoad.flag = 0;

    if ((int)vectorOfNeighboringAPs.size() != 0){
      for (int i=0;i<(int)vectorOfNeighboringAPs.size();i++){
        RemoveLoad.header.destinationID = vectorOfNeighboringAPs.at(i);
        outLoadToNeighbor(RemoveLoad);
      }
    }

    channel_reward.push_back(std::max(0.0,1-(double)(trafficLoad/100)));
    Action_Selected.push_back(actionSelected);

    if (trafficLoad < 100){
      occupanyOfAp.push_back(trafficLoad);
      TimeSimulation.push_back(SimTime());
    }
    else{
      occupanyOfAp.push_back((double)100);
      TimeSimulation.push_back(SimTime());
    }
  }
}

void AP :: inLoadFromNeighbor(ApNotification &notification){

  if (apID == notification.header.destinationID){

    int Neighbor_CH_index;
    for (int i=0; i<(int)CHMapToAction.size(); i++){
      if (notification.ChannelNumber == CHMapToAction.at(i)){
        Neighbor_CH_index = i;
      }
    }

    int overlapping = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,notification.ChannelNumber);

    if (notification.flag == 1){
      if (overlapping == 1){
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)+notification.Load;
        trafficLoad = trafficLoad + notification.Load;
      }
      else{
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)+notification.Load;
      }
    }
    if (notification.flag == 0){
      if (overlapping == 1){
        trafficLoad = trafficLoad - notification.Load;
        if (trafficLoad < 0.00001){
          trafficLoad = 0;
        }
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)-notification.Load;
        if (CH_occupancy_detected.at(Neighbor_CH_index) < 0.00001){
          CH_occupancy_detected.at(Neighbor_CH_index) = 0;
        }
      }
      else{
        CH_occupancy_detected.at(Neighbor_CH_index) = CH_occupancy_detected.at(Neighbor_CH_index)-notification.Load;
        if (CH_occupancy_detected.at(Neighbor_CH_index) < 0.00001){
          CH_occupancy_detected.at(Neighbor_CH_index) = 0;
        }
      }
    }


    channel_reward.push_back(std::max(0.0,1-(double)(trafficLoad/100)));
    Action_Selected.push_back(actionSelected);

    if (trafficLoad < 100){
      occupanyOfAp.push_back(trafficLoad);
      TimeSimulation.push_back(SimTime());
    }
    else{
      occupanyOfAp.push_back((double)100);
      TimeSimulation.push_back(SimTime());
    }

    if (trafficLoad >= 100){
      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        Connection update;
        update.header.sourceID = apID;
        update.header.destinationID = vectorOfConnectedStations.at(i);
        update.Ap_Load = trafficLoad;
        outAssignAirTime(update);
      }
    }
  }
}

void AP :: inUpdateConnection(StationInfo &info, int oldAP){

  if (apID == oldAP){
    for (int i=0; i<(int)vectorOfConnectedStations.size(); i++){
      if (vectorOfConnectedStations.at(i) == info.header.sourceID){
        vectorOfConnectedStations.erase(vectorOfConnectedStations.begin()+i);
        vectorOfConnectedStationsRSSIs.erase(vectorOfConnectedStationsRSSIs.begin()+i);
        numOfConnectedStations = numOfConnectedStations - 1;
      }
    }
  }

  if (apID == info.header.destinationID){
    numOfConnectedStations++;
    vectorOfConnectedStations.push_back(info.header.sourceID);
    vectorOfConnectedStationsRSSIs.push_back(info.RSSI);

    APBeacon beacon;
    beacon.header.destinationID = info.header.sourceID;
    beacon.header.sourceID = apID;
    beacon.header.X = X;
    beacon.header.Y = Y;
    beacon.header.Z = Z;
    beacon.Tx_Power = txPower;
    beacon.freq = frequency;
    beacon.protocolType = IEEEprotocolType;
    beacon.BW = channelBW;

    outChannelChange(beacon);
  }
}

void AP :: inUpdateAttachedStationsParams (StationInfo &info){

  if (info.header.destinationID == apID){
    for (int i=0; i<(int)vectorOfConnectedStations.size(); i++){
      if (info.header.sourceID == vectorOfConnectedStations.at(i)){
        vectorOfConnectedStationsRSSIs.at(i) = info.RSSI;
      }
    }
  }
}

void AP :: CHselectionBylearning(trigger_t&){

  int lastAction = actionSelected;
  int num_arms = setOfactions.size();
  int index = SearchAction(lastAction, num_arms, &setOfactions);

  switch (APlearningFlag) {

    case 1:{

      if (flag == 0){
        int i = rand()%num_arms;
        actionSelected = setOfactions.at(i);
        TimesActionIsPicked[i] = TimesActionIsPicked[i] + 1;

        flag++;
      }
      else{
        double epsilon = 1/sqrt(iter);
        reward_action[index] = GetReward(lastAction, &channel_reward, &Action_Selected, &TimeSimulation, SimTime());
        occupancy_CH[index] = GetOccupancy(lastAction, &occupanyOfAp, &Action_Selected, &TimeSimulation, SimTime());

        actionSelected = setOfactions.at(Egreedy(num_arms, &reward_action, &occupancy_CH, epsilon, &TimesActionIsPicked));
      }

      for (int i=0; i<(int)CHMapToAction.size();i++){
        int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
        if (overlap ==1){
          trafficLoad = trafficLoad-CH_occupancy_detected.at(i);
          if (trafficLoad < 0.00001){
            trafficLoad = 0;
          }
        }
      }

      double* selectedConfiguration = GetConfiguration(IEEEprotocolType, actionSelected);
      channelBW = *selectedConfiguration;
      OperatingChannel = *(selectedConfiguration+1);
      frequency = *(selectedConfiguration+2);

      if ((int)vectorOfNeighboringAPs.size() != 0){
        for (int n=0;n<(int)vectorOfNeighboringAPs.size();n++){
          if(trafficLoad != 0){

            ApNotification RemoveLoad;
            RemoveLoad.Load = trafficLoad;
            RemoveLoad.ChannelNumber = CHMapToAction.at(index);
            RemoveLoad.flag = 0;
            RemoveLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
            outLoadToNeighbor(RemoveLoad);


            ApNotification AddLoad;
            AddLoad.Load = trafficLoad;
            AddLoad.ChannelNumber = OperatingChannel;
            AddLoad.flag = 1;
            AddLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
            outLoadToNeighbor(AddLoad);
          }
        }
      }

      for (int i=0; i<(int)CHMapToAction.size();i++){
        int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
        if (overlap ==1){
          trafficLoad = trafficLoad+CH_occupancy_detected.at(i);
        }
      }

      for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
        APBeacon beacon;
        beacon.header.destinationID = vectorOfConnectedStations.at(i);
        beacon.header.sourceID = apID;
        beacon.header.X = X;
        beacon.header.Y = Y;
        beacon.header.Z = Z;
        beacon.Tx_Power = txPower;
        beacon.freq = frequency;
        beacon.protocolType = IEEEprotocolType;
        beacon.BW = channelBW;
        outChannelChange(beacon);

        Connection update;
        update.header.sourceID = apID;
        update.header.destinationID = vectorOfConnectedStations.at(i);
        update.Ap_Load = trafficLoad;
        outAssignAirTime(update);
      }

      ActionChange.push_back(actionSelected);
      TimeStamp.push_back(SimTime());

      trigger_Action.Set(SimTime()+ChSelectionTime);
      iter++;

    }break;

    case 2:{
      if (flag == 0){
        int i = rand()%num_arms;
        actionSelected = setOfactions.at(i);
        TimesActionIsPicked[i] = TimesActionIsPicked[i] + 1;

        flag++;
      }
      else{

        reward_action[index] = GetReward(lastAction, &channel_reward, &Action_Selected, &TimeSimulation, SimTime());
        occupancy_CH[index] = GetOccupancy(lastAction, &occupanyOfAp, &Action_Selected, &TimeSimulation, SimTime());
        estimated_reward_action[index] = ((estimated_reward_action[index] * TimesActionIsPicked[index]) + (reward_action[index])) / (TimesActionIsPicked[index] + 2);
        estimated_reward_Per_action[index].push_back(estimated_reward_action[index]);
        estimated_reward_Per_action_Time[index].push_back(SimTime());
        actionSelected = setOfactions.at(ThompsonSampling(num_arms, &estimated_reward_action, &occupancy_CH, &TimesActionIsPicked));

        for (int i=0; i<(int)CHMapToAction.size();i++){
          int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
          if (overlap ==1){
            trafficLoad = trafficLoad-CH_occupancy_detected.at(i);
            if (trafficLoad < 0.00001){
              trafficLoad = 0;
            }
          }
        }

        double* selectedConfiguration = GetConfiguration(IEEEprotocolType, actionSelected);
        channelBW = *selectedConfiguration;
        OperatingChannel = *(selectedConfiguration+1);
        frequency = *(selectedConfiguration+2);

        if ((int)vectorOfNeighboringAPs.size() != 0){
          for (int n=0;n<(int)vectorOfNeighboringAPs.size();n++){
            if(trafficLoad != 0){

              ApNotification RemoveLoad;
              RemoveLoad.Load = trafficLoad;
              RemoveLoad.ChannelNumber = CHMapToAction.at(index);
              RemoveLoad.flag = 0;
              RemoveLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
              outLoadToNeighbor(RemoveLoad);


              ApNotification AddLoad;
              AddLoad.Load = trafficLoad;
              AddLoad.ChannelNumber = OperatingChannel;
              AddLoad.flag = 1;
              AddLoad.header.destinationID = vectorOfNeighboringAPs.at(n);
              outLoadToNeighbor(AddLoad);
            }
          }
        }

        for (int i=0; i<(int)CHMapToAction.size();i++){
          int overlap = ChannelOverlappingDetector(IEEEprotocolType,OperatingChannel,CHMapToAction.at(i));
          if (overlap ==1){
            trafficLoad = trafficLoad+CH_occupancy_detected.at(i);
          }
        }

        for (int i=0;i<(int)vectorOfConnectedStations.size();i++){
          APBeacon beacon;
          beacon.header.destinationID = vectorOfConnectedStations.at(i);
          beacon.header.sourceID = apID;
          beacon.header.X = X;
          beacon.header.Y = Y;
          beacon.header.Z = Z;
          beacon.Tx_Power = txPower;
          beacon.freq = frequency;
          beacon.protocolType = IEEEprotocolType;
          beacon.BW = channelBW;
          outChannelChange(beacon);

          Connection update;
          update.header.sourceID = apID;
          update.header.destinationID = vectorOfConnectedStations.at(i);
          update.Ap_Load = trafficLoad;
          outAssignAirTime(update);
        }

        ActionChange.push_back(actionSelected);
        TimeStamp.push_back(SimTime());

        trigger_Action.Set(SimTime()+ChSelectionTime);
      }
    }break;
  }
}

void AP :: ProgressFunct(trigger_t&){
  printf("Progress: %f\n", (SimTime()/(double)86400)*100);
  trigger_progress.Set(SimTime()+4320);
}

#endif
