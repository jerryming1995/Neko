#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <list>

#include "../COST/cost.h"
#include "../Config/constants.h"
#include "../Methods/frequency.h"
#include "../Methods/modulation.h"
#include "../Classes/structs.h"
#include "../Methods/pathloss.h"
#include "../Methods/general.h"
#include "../Learning/strategies.h"
#include "../Classes/flow.cc"
#include "../Classes/notification.cc"
#include "../Classes/interface.h"
#include "../Classes/agent.cc"
#include "../Classes/policy_manager.cc"
#include "application.h"
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
	void GenerateResultReport();
	//void LoadScenario();

public:
	int seed;

	AP [] APContainer;																							//AP array that serves as container of the AP class node.
  STA [] STAContainer;																						//STA array that serves as container of the STA class node.
	Application [] AppContainer;																		//APP array that serves as container of the Application class node.

	Timer <trigger_t> Progress;
	inport inline void ProgressBar(trigger_t&);
};

/* ----------------------------------------------------------------------------------
Within the Setup function, the scenario is generated and the component connections
are created. Every node has a connection with everyone else to ensure that
upon changes the simulations runs correctly.
---------------------------------------------------------------------------------- */

void Neko :: Setup(){

	if (rnd){
		GenerateRandom();
	}

	for (int i=0; i<(int)APContainer.size(); i++){
		for (int j=0; j<(int)STAContainer.size(); j++){
			connect APContainer[i].outCtrlSTA,STAContainer[j].inCrtlAP;
			connect APContainer[i].outDataSTA,STAContainer[j].inDataAP;
			connect STAContainer[j].outCtrlAP,APContainer[i].inCtrlSTA;
		}

		for (int j=0; j<(int)APContainer.size(); j++){
			if (APContainer[i].apID != APContainer[j].apID){
				connect APContainer[i].outCtrlAP, APContainer[j].inCtrlAP;
			}
		}
		for (int j=0; j<(int)AppContainer.size();j++){
			connect APContainer[i].outCtrlApp,AppContainer[j].inCtrlAP;
			connect AppContainer[j].outCtrlAP,APContainer[i].inCtrlApp;
		}
	}
	connect Progress.to_component,ProgressBar;
	Progress.Set(SimTime());
}

void Neko :: Start(){

	//Do nothing
}

void Neko :: Stop(){
	GenerateResultReport();
}

/* ----------------------------------------------------------------------------------
Function to generate a random scenario. Within this function parameters such as the
number of APs and STAs are configured, as well as Position, Ptx, MLO capabilities or
learning agents.
---------------------------------------------------------------------------------- */

void Neko :: GenerateRandom(){

	std::default_random_engine generator(rand());
	bool position;
	double Xaxis = 40;
	double Yaxis = 30;
	int numOfAPs = 3;
	int stationsPerAP = 25;
	std::vector<int> stations(numOfAPs,0.0);

	int numOfStations = 0;
	std::uniform_int_distribution<int> UStas(15, stationsPerAP);
	for (int i=0; i<(int)stations.size(); i++){
		stations.at(i) = UStas(generator);
		numOfStations += stations.at(i);
	}

	APContainer.SetSize(numOfAPs);
	STAContainer.SetSize(numOfStations);
	AppContainer.SetSize(numOfStations);

	int index = 0;
	for (int i=0; i<numOfAPs; i++){

		//Frequencies auto assigned upon interface creation inside constructor AP
		APContainer[i].apID = i;
		APContainer[i].configuration.TxPower = 20;
		APContainer[i].configuration.CCA = -82;
		APContainer[i].configuration.nSS = 2;
		APContainer[i].capabilities.IEEEProtocol = 2;
		APContainer[i].capabilities.Multilink = true;
		APContainer[i].capabilities.Mlearning = false;

		//std::uniform_real_distribution<double> distX(Dmax(APContainer[i].configuration.CCA, APContainer[i].configuration.TxPower, 2.462), (Xaxis - Dmax(APContainer[i].configuration.CCA, APContainer[i].configuration.TxPower, 2.462))); //Max (fc,Bw) combination for the 2.4GHz band
		//std::uniform_real_distribution<double> distY(Dmax(APContainer[i].configuration.CCA, APContainer[i].configuration.TxPower, 2.462), (Yaxis - Dmax(APContainer[i].configuration.CCA, APContainer[i].configuration.TxPower, 2.462))); //Max (fc,Bw) combination for the 2.4GHz band

		std::uniform_real_distribution<double> distX(0,Xaxis);
		std::uniform_real_distribution<double> distY(0,Yaxis);

		//APContainer[i].coordinates.x = distX(generator);
		APContainer[i].coordinates.x = (i == 0) ? 10 : APContainer[i].coordinates.x = APContainer[i-1].coordinates.x + 10;
		//APContainer[i].coordinates.y = distY(generator);
		APContainer[i].coordinates.y = 15;
		APContainer[i].coordinates.z = 3;

		//Intersite euclidean distance of 5m
		position = false;
		while (!position){
			if (i == 0){
				position = true;
			}
			else{
				for (int n=0; n<i; n++){
					double distance = sqrt(pow((APContainer[n].coordinates.x - APContainer[i].coordinates.x),2) + pow((APContainer[n].coordinates.y - APContainer[i].coordinates.y),2) + pow((APContainer[n].coordinates.z - APContainer[i].coordinates.z),2));
					if (distance <= 5.0){
						APContainer[i].coordinates.x = distX(generator);
						APContainer[i].coordinates.y = distY(generator);
						n=0;
					}
					else{
						position = true;
					}
				}
			}
		}

		std::uniform_real_distribution<double> rad(1.0, 8.0); //Max distance for worst case scenario (highest fc, highest ChW) for the 2.4GHz band
		for (int j=0; j<stations.at(i); j++){
			double r = rad(generator);
			double theta = 2*M_PI*drand48();
			AppContainer[index].destID = index;
			STAContainer[index].staID = index;
			STAContainer[index].servingAP = APContainer[i].apID;
			STAContainer[index].traffic_type = (Random()>0.8) ? "STREAMING" : "ELASTIC";
			//STAContainer[index].traffic_type = "ELASTIC";
			STAContainer[index].coordinates.x = APContainer[i].coordinates.x + r*std::cos(theta);
			STAContainer[index].coordinates.y = APContainer[i].coordinates.y + r*std::sin(theta);
			STAContainer[index].coordinates.z = 1.5;
			STAContainer[index].configuration.TxPower = 15;
			STAContainer[index].configuration.CCA = -82;
			STAContainer[index].configuration.nSS = 2;
			STAContainer[index].capabilities.IEEEProtocol = 2;
			STAContainer[index].capabilities.Multilink = true;
			STAContainer[index].capabilities.Mlearning = false;

			WifiSTA station;
			station.id = index;
			station.coord.x = STAContainer[index].coordinates.x;
			station.coord.y = STAContainer[index].coordinates.y;
			station.coord.z = STAContainer[index].coordinates.z;
			station.traffic_type = STAContainer[index].traffic_type;
			APContainer[i].AssociatedSTAs.push_back(station);

			index++;
		}
	}
}

/* ----------------------------------------------------------------------------------
This function gathers all the statistics from the nodes and saves it to two different
files:

- ChOccupancy -> registers the average occupancy of each AP for each of their interfaces.
- Stats --> registers the average satisfaction and drop ratio experienced by all the stations,
as well as the aggregated throughput.
---------------------------------------------------------------------------------- */

void Neko::GenerateResultReport(){

	/* Generate channel occupancy report for each AP*/
	if (channel_report){
		FILE* OccupancyReport = fopen("../Output/ChOccupancy.txt","a");
		fprintf(OccupancyReport, "*******************\n");
		fprintf(OccupancyReport, "Simultation seed: %i\n", seed);
		fprintf(OccupancyReport, "AP id; ChOcc 2_4; ChOcc5; ChOcc6\n");

		for (int i=0; i<(int)APContainer.size(); i++){
			fprintf(OccupancyReport, "%i; ", APContainer[i].apID);
			std::vector<double> AvgChOcc(maxIntNum, 0.0);
			for (int j=0; j<(int)APContainer[i].statistics.ChOcc.size(); j++){
				for (int n=0; n<(int)APContainer[i].statistics.ChOcc[j].size(); n++){
					if (APContainer[i].statistics.ChOcc[j].at(n) != -1){
						AvgChOcc.at(n) += APContainer[i].statistics.ChOcc[j].at(n);
					}
				}
			}
			for (int j=0; j<(int)AvgChOcc.size(); j++){
				if (AvgChOcc.at(j) != 0){
					AvgChOcc.at(j) = AvgChOcc.at(j)/(int)APContainer[i].statistics.SimT.size();
				}
				if(j<(int)AvgChOcc.size()-1){
					fprintf(OccupancyReport, "%f; ", AvgChOcc.at(j));
				}
				else{
					fprintf(OccupancyReport, "%f", AvgChOcc.at(j));
				}
			}
			fprintf(OccupancyReport, "\n");
		}
	}
	if (stats_report){
		FILE* StatsReport = fopen("../Output/Stats.txt","a");
		fprintf(StatsReport, "*******************\n");
		fprintf(StatsReport, "Simultation seed: %i\n", seed);
		fprintf(StatsReport, "Avg_Sat; Avg_Th; Avg_Dratio\n");

		double TotalSatAvg = 0, TotalThAvg = 0;
		for (int i=0; i<(int)STAContainer.size(); i++){
			TotalSatAvg += std::accumulate(STAContainer[i].statistics.AvgSatPerFlow.begin(), STAContainer[i].statistics.AvgSatPerFlow.end(),0.0)/(int)STAContainer[i].statistics.AvgSatPerFlow.size();
			TotalThAvg += std::accumulate(STAContainer[i].statistics.AvgThPerFlow.begin(), STAContainer[i].statistics.AvgThPerFlow.end(),0.0)/(int)STAContainer[i].statistics.AvgThPerFlow.size();
		}
		fprintf(StatsReport, "%f; %f; ", TotalSatAvg/(int)STAContainer.size(), TotalThAvg);

		double TotalAvgDrop = 0;
		for (int i=0; i<(int)APContainer.size(); i++){
			TotalAvgDrop += std::accumulate(APContainer[i].statistics.AvgDRPerFlow.begin(), APContainer[i].statistics.AvgDRPerFlow.end(),0.0)/(double)APContainer[i].statistics.AvgDRPerFlow.size();
		}
		fprintf(StatsReport, "%f", TotalAvgDrop/(int)APContainer.size());
		fprintf(StatsReport, "\n");
	}
}

void Neko::ProgressBar(trigger_t&){

	int now = (int)SimTime();
	double t = runTime;
	double progress = now/t;

	int barWidth = 50;
	int pos = barWidth * progress;
	std::cout << "[";

	for (int i = 0; i < barWidth; i++) {
				if (i < pos) std::cout << "=";
				else if (i == pos) std::cout << ">";
				else std::cout << " ";
		}

	std::cout << "] " << int(progress * 100.0) << "%\r";
	std::cout.flush();

	if (progress < 100){
		Progress.Set(SimTime()+9);
	}
}

int main(int argc, char *argv[]){

	std::cout << "************* SIMULATION STARTS *************" << std::endl;
	Neko test;

	if (rnd == 1) {
		test.seed = atoi(argv[1]);
		medBW = atof(argv[2]);
		policy = argv[3];
		srand(test.seed);
	}
	else{
		test.seed=time(NULL);
		srand(test.seed);
	}

	test.StopTime(runTime);
	test.Setup();
	test.Run();

	std::cout << "*********************************************" << std::endl;
}
