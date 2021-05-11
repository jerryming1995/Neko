
/*Implementation file for the Flow class*/

#include "flow.h"


Flow::Flow(){

}

Flow::~Flow(){

}

void Flow::setType(std::string type){

	_type = type;
}

void Flow::setSender(int sender){

	_sender = sender;
}

void Flow::setDestination(int destination){

	_destination = destination;
}

void Flow::setDuration(double duration){

	_duration = duration;
}

void Flow::setTimeStamp(double timestamp){

	_timestamp = timestamp;
}

void Flow::setFc(double fc){

	_Fc.push_back(fc);
}

void Flow::setTxTime(double t){

	_TxTime.push_back(t);
}

void Flow::setLength(double dlength){

	_length = dlength;
}

void Flow::setSat(std::vector<double> &s){

	if ((int)_Sat.size() == 0){
		for (int i=0; i<(int)s.size(); i++){
			std::vector<double> inter;
			_Sat.push_back(inter);
		}
	}
	for (int i=0; i<(int)s.size(); i++){
		_Sat[i].push_back(s.at(i));
	}
}

std::string Flow::getType(){

	return _type;
}

int Flow::getSender(){

	return _sender;
}

int Flow::getDestination(){

	return _destination;
}

std::vector<double> Flow::getTxTime(){

	return _TxTime;
}

std::vector<double> Flow::getFc(){

	return _Fc;
}

double Flow::getDuration(){

	return _duration;
}

double Flow::getTimeStamp(){

	return _timestamp;
}

double Flow::getLength(){

	return _length;
}

/*----------------------------------------------------------------------------------------------------------------------------*/
/*Update the Fc which the flow is being transmitted at*/
void Flow::UpdateFc(std::vector<double> &old_fc, std::vector<double> &new_fc){
	//UPDATE TX TIMES ACCORDING TO NEW FC DISTRIBUTION  <--------- INVESTIGATE. Notation or comment?? Ok, for policy dynamic.
	for (int i=0; i<(int)_Fc.size(); i++){
		for (int j=0; j<(int)old_fc.size(); j++){
			if (old_fc.at(j) == _Fc.at(i)){
				_Fc.at(i) = new_fc.at(j);
				break;
			}
		}
	}
}
/*----------------------------------------------------------------------------------------------------------------------------*/

/*Update the Tx times according to the new ones passed. Executed when dynamic policies or change of APs*/
void Flow::UpdateTxTime(std::vector<double> &new_times){
	_TxTime.clear();
	for (int i=0; i<(int)new_times.size(); i++){
		_TxTime.push_back(new_times.at(i));
	}
}


/*Get the avg drop ratio experienced by the flow*/
double Flow::getDratio(){

	int num_interfaces = 0, counter = 0;
	double avg_flow = 0, sat_interface = 0, dropRatio = 0;
	for (int i=0; i<(int)_Sat.size(); i++){
		for (int j=0; j<(int)_Sat[i].size(); j++){
			if (_Sat[i].at(j) != -1){
				sat_interface += _Sat[i].at(j);   //Accumulate all the entries for the i_th interface
				counter++;											  //Count the number of entries registered during the flow for that interface
			}
		}

		if (sat_interface != 0){
			avg_flow += sat_interface/counter;						//Average the satisfaction values registered for the i_th interface.
			num_interfaces++;															//Counter to add the number of interfaces involved during the flow transmission
		}
		sat_interface = 0, counter=0;                               //reset parameter for next interface.
	}

	dropRatio = 1 - ((_length*(avg_flow/num_interfaces))/_length);				//Calculate the flow drop ratio.
	return dropRatio;
}

/*Get the avg satisfaction experienced by the flow*/
double Flow::getSatisfaction(){

  int num_interfaces = 0, counter = 0;
	double avg_flow = 0, sat_interface = 0, avg_satisfaction = 0;
	for (int i=0; i<(int)_Sat.size(); i++){
		for (int j=0; j<(int)_Sat[i].size(); j++){
			//std::cout << "dest: " << _destination << " size: " << _Sat[i].size() << std::endl;
			if (_Sat[i].at(j) != -1){
				sat_interface += _Sat[i].at(j);   //Accumulate all the entries for the i_th interface
				counter++;											  //Count the number of entries registered during the flow for that interface
			}
		}

		if (sat_interface != 0){
			avg_flow += sat_interface/counter;						//Average the satisfaction values registered for the i_th interface.
			num_interfaces++;															//Counter to add the number of interfaces involved during the flow transmission
		}
		sat_interface = 0, counter=0;                               //reset parameter for next interface.
	}
  avg_satisfaction = avg_flow/num_interfaces;				//Calcultae the flow drop ratio.
	return avg_satisfaction;
}
