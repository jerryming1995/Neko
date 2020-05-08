#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <string>

#include "../COST/cost.h"
#include "../Main/macros.h"
#include "../Methods/freq_and_modulation.h"
#include "../Methods/pathloss.h"
#include "../Methods/helpers.h"
#include "../Learning/strategies.h"
#include "../Structures/notifications.h"
#include "ap.h"
#include "station.h"

component Neko : public CostSimEng {

public:

  //COST
  void Setup();
  void Start();
  void Stop();

  //Functions
  void GenerateRandom();
  void LoadScenario();

public:

  AP [] APoint_container;
  Station [] STA_container;

  int seed;

  int numOfAPs;
  int numOfStations;
};

void Neko :: Setup(){

  if (rnd == 1){
    GenerateRandom();
  }
  else{
    LoadScenario();
  }

  for (int j=0; j<numOfStations; j++){
    for (int i=0; i<numOfAPs; i++){

        connect STA_container[j].outSetClientAssociation,APoint_container[i].inSetClientAssociation;
        connect STA_container[j].outRequestAirTime,APoint_container[i].inRequestedAirTime;
        connect STA_container[j].outFlowEnded,APoint_container[i].inTxTimeFinished;
        connect STA_container[j].outUpdateConnection,APoint_container[i].inUpdateConnection;
        connect STA_container[j].outUpdateAttachedStationsParams,APoint_container[i].inUpdateAttachedStationsParams;

        connect APoint_container[i].outSendBeaconToNodes,STA_container[j].inReceivedBeacon;
        connect APoint_container[i].outAssignAirTime,STA_container[j].inAssignedAirTime;
        connect APoint_container[i].outChannelChange,STA_container[j].inUpdateStationParameters;
    }
  }

  for (int i=0;i<numOfAPs;i++){
    for (int j=0; j<numOfAPs; j++){
      if (APoint_container[i].apID != APoint_container[j].apID){
        connect APoint_container[i].outLoadToNeighbor,APoint_container[j].inLoadFromNeighbor;
        connect APoint_container[i].outSetNeighbors,APoint_container[j].inSetNeighbors;
      }
    }
  }
}

void Neko :: Start(){

  //Do nothing
}

void Neko :: Stop(){

  FILE* pFileSTATS = fopen("../Output/STATS.txt","a");
  float tmpSat, tmpTh, tmpDRatio, simSat, simTh, simDRatio;
  std::vector<double> mean_Satisfaction;
  double m_val = 0;

  for (int i=0; i<(int)STA_container[0].mSat.size(); i++){
    for (int j=0; j<numOfStations; j++){
      m_val += STA_container[j].mSat.at(i);
    }

    mean_Satisfaction.push_back(m_val/numOfStations);
    m_val = 0;
  }

  simTh = 0;
  simDRatio = 0;
  simSat = 0;

  tmpSat = 0;
  tmpTh = 0;
  tmpDRatio = 0;

  for (int i=0; i<(int)mean_Satisfaction.size(); i++){
    tmpSat += mean_Satisfaction.at(i);
  }

  simSat = tmpSat/(int)mean_Satisfaction.size();

  for (int i=0; i<numOfStations; i++){
    tmpTh += std::accumulate(STA_container[i].Throughput.begin(),STA_container[i].Throughput.end(), 0.0)/(int)STA_container[i].Throughput.size();
    tmpDRatio += 1-(STA_container[i].bits_Sent/STA_container[i].totalBits);
  }

  simTh = tmpTh;
  simDRatio = tmpDRatio/numOfStations;
  printf("Th: %f\n", simTh);
  fprintf(pFileSTATS, "%i; %f; %f; %f\n", seed, simSat, simTh, (simDRatio*100));

  fclose(pFileSTATS);
}

void Neko :: GenerateRandom(){

  int Xaxis = 30;
  int Yaxis = 30;
  int Zaxis = 2;
  int index = 0;
  int stationsPerAP = 15;

  numOfAPs = 20;
  numOfStations = numOfAPs*stationsPerAP;

  APoint_container.SetSize(numOfAPs);
  STA_container.SetSize(numOfStations);

  int user = 0;

  for (int i=0; i<numOfAPs; i++){
    APoint_container[i].apID = i;
    APoint_container[i].X = ((double)rand() / RAND_MAX) * Xaxis;
    APoint_container[i].Y = ((double)rand() / RAND_MAX) * Yaxis;
    APoint_container[i].Z = ((double)rand() / RAND_MAX) * Zaxis;
    APoint_container[i].txPower = 15;
    APoint_container[i].CCA = -80;
    APoint_container[i].actionSelected = rand()%3;
    APoint_container[i].IEEEprotocolType = 2;
    APoint_container[i].trafficLoad = 0;


    for (int j=0; j<stationsPerAP; j++){
      int k = 0;
      while (k<1){
        double tmpX = ((double)rand() / RAND_MAX) * Xaxis;
        double tmpY = ((double)rand() / RAND_MAX) * Yaxis;
        double tmpZ = ((double)rand() / RAND_MAX) * Zaxis;

        float propL = PropL(tmpX,tmpY,tmpZ,APoint_container[i].X,APoint_container[i].Y,APoint_container[i].Z, 5.32);
        float RSSI = 15-propL;

        if ((-80 <= RSSI) && (RSSI < -45)){
          STA_container[index].staID = index;
          STA_container[index].X = tmpX;
          STA_container[index].Y = tmpY;
          STA_container[index].Z = tmpZ;
          STA_container[index].txPower = 15;

          double rndProbability = ((double) rand() / (RAND_MAX));

          if (rndProbability <= ProbUserAgentEnabled){
            STA_container[index].userType = 2;
            user++;
          }
          else{
            STA_container[index].userType = 1;
          }
          //printf("STA ID: %i, propL: %f, RSSI: %f\n", index, propL, RSSI);
          k++;
          index++;
        }
      }
    }
  }
  printf("user: %i\n", user);
}

void Neko :: LoadScenario(){

  FILE* inputFileName = fopen("../Input/Inputfile.txt", "r");
  char line [100];
  char *str;
  char ap[] = "AP";
  char sta[] = "STA";
  int i = 0;
  int j = 0;

  numOfAPs = 0;
  numOfStations = 0;

  fgets(line,100,inputFileName); // Line to escape headers line

  while ((fgets(line,100,inputFileName))){
    str = strtok(line, ";");
    if (strcmp(ap, str) == 0){
      numOfAPs++;
    }
    else if(strcmp(sta, str) == 0){
      numOfStations++;
    }
  }

  APoint_container.SetSize(numOfAPs);
  STA_container.SetSize(numOfStations);

  rewind (inputFileName);
  fgets(line,100,inputFileName); // Line to escape headers line

  while ((fgets(line,100,inputFileName))){
    str = strtok(line, ";");

    if (strcmp(ap, str) == 0){
      APoint_container[i].apID = i;
      APoint_container[i].X = atof(strtok(NULL, ";"));
      APoint_container[i].Y = atof(strtok(NULL, ";"));
      APoint_container[i].Z = atof(strtok(NULL, ";"));
      APoint_container[i].txPower = atoi(strtok(NULL, ";"));
      APoint_container[i].CCA = atof(strtok(NULL, ";"));
      APoint_container[i].actionSelected = atoi(strtok(NULL, ";"));
      APoint_container[i].IEEEprotocolType = atoi(strtok(NULL, ";"));
      APoint_container[i].trafficLoad = atof(strtok(NULL, ";\n"));
      i++;
    }
    else if(strcmp(sta, str) == 0){
      STA_container[j].staID = j;
      STA_container[j].X = atof(strtok(NULL, ";"));
      STA_container[j].Y = atof(strtok(NULL, ";"));
      STA_container[j].Z = atof(strtok(NULL, ";"));
      STA_container[j].txPower = atoi(strtok(NULL, ";"));
      STA_container[j].userType = atoi(strtok(NULL, ";"));
      j++;
    }
  }
  fclose(inputFileName);
}

int main(int argc, char *argv[]){

  Neko test;

  if (rnd == 1) {
    test.seed = atoi(argv[1]);
    test.Seed = test.seed;
    srand(test.seed);
  }
  else{
    srand(time(NULL));
    test.Seed=rand()%100;
  }

  test.StopTime(runTimeSim);
  test.Setup();
  test.Run();
}
