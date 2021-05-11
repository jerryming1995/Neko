/*
Notifications used for communication between AP-AP, AP-STA and STA-AP.

Notification:
We define different notifications dependind on the message to be exchanged. Types:
	- PROBE_REQ/RESP
	- ASSOCIATION_REQ/RESP
	- MLO_SETUP_REQ/RESP
	- CHANNEL_SWITCH
	- AP_LOAD
	- AP_NEIGHBOR_DISCOVERY
	- STA_DEASSOCIATION
*/

class Notification{

private:
	std::string _type;												//Notification type.
	std::string _flag;												//Operation flag (only used for ADD or REMOVE airtime from APs)

	int _sender;															//Sender ID.
	int _destination;													//Destination ID.

	Capabilities _capabilities;								//Capabilities of the node
	Position _coordinates;										//Position of the node

	std::vector<double> _linkfc;							//Vector of Fc used by the AP
	std::vector<double> _linkQ;								//Link qualities

	std::vector<double> _ChOcc;								//Ch occupancy
	std::vector<double> _Sat;									//Flow satisfaction


public:
	Notification(std::string t, int s, int d):_type(t), _sender(s), _destination(d){};

	//Setters
	void setPosition(Position &p);
	void setCapabilities(Capabilities &cap);
	void setFlag(std::string);
	void setFc(double);
	void setFc(std::vector<double> &v);
	void setLinkQuality(std::vector<double> &v);
	void setChOcc(std::vector<double> &v);
	void setSat(std::vector<double> &v);

	//Getters
	std::string getType();
	std::string getFlag();
	int getSender();
	int getDestination();
	Position getPosition();
	Capabilities getCapabilities();
	std::vector<double> getFc();
	std::vector<double> getLinkQuality();
	std::vector<double> getSat();
	std::vector<double> getChOcc();
};

// --------------------------------------------------------------------------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------------------------------------------------------------------------//

class AppCTRL{

private:

	std::string _type;

	int _sender;
	int _destination;
	double _duration;
	
	std::string _tprofile;

public:

	AppCTRL(std::string t, int s, int d): _type(t), _sender(s), _destination(d){}

	//Setters
	void setTProfile(std::string);
	void setDuration(double);
	//Getters
	std::string getType();
	int getSender();
	int getDestination();
	std::string getTProfile();
	double getDuration();
};
