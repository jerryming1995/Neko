
#include "notification.h"

//Setters
void Notification::setPosition(Position &p){

	_coordinates.x = p.x;
	_coordinates.y = p.y;
	_coordinates.z = p.z;
}

void Notification::setCapabilities(Capabilities &c){

	_capabilities.IEEEProtocol = c.IEEEProtocol;
	_capabilities.Multilink = c.Multilink;
	_capabilities.Mlearning = c.Mlearning;
}

void Notification::setLinkQuality(std::vector<double> &v){

	for (int i=0; i<(int)v.size(); i++){
		_linkQ.push_back(v.at(i));
	}
}

void Notification::setFc(std::vector<double> &v){

	for (int i=0; i<(int)v.size(); i++){
		_linkfc.push_back(v.at(i));
	}
}

void Notification::setFc(double fc){

		_linkfc.push_back(fc);
}

void Notification::setFlag(std::string flag){

	_flag = flag;
}
void Notification::setSat(std::vector<double> &sat){

	for (int i=0; i<(int)sat.size(); i++){
		_Sat.push_back(sat.at(i));
	}
}

void Notification::setChOcc(std::vector<double> &occ){

	for (int i=0; i<(int)occ.size(); i++){
		_ChOcc.push_back(occ.at(i));
	}
}

//Getters
std::string Notification::getType(){

	return _type;
}

int Notification::getSender(){

	return _sender;
}

int Notification::getDestination(){

	return _destination;
}

Position Notification::getPosition(){

	return _coordinates;
}

Capabilities Notification::getCapabilities(){

	return _capabilities;
}

std::vector<double> Notification::getLinkQuality(){

	return _linkQ;
}

std::vector<double> Notification::getFc(){

	return _linkfc;
}
std::string Notification::getFlag(){

	return _flag;
}

std::vector<double> Notification::getSat(){

	return _Sat;
}

std::vector<double> Notification::getChOcc(){

	return _ChOcc;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------------------------------------------------------------------------//

void AppCTRL::setTProfile(std::string p){

	_tprofile = p;
}

void AppCTRL::setDuration(double t){

	_duration = t;
}

std::string AppCTRL::getType(){

	return _type;
}

int AppCTRL::getSender(){

	return _sender;
}

int AppCTRL::getDestination(){

	return _destination;
}

std::string AppCTRL::getTProfile(){

	return _tprofile;
}

double AppCTRL::getDuration(){

	return _duration;
}
