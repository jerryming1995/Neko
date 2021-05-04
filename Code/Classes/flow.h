

/* Flow class */

class Flow{

private:

	std::string _type;															//Type of flow (elastic or streaming)
	int _sender;																		//Source node. Tx of the flow
	int _destination;																//Destination node. Rx of the flow
	double _timestamp;
	double _length;																	//Length. For elastic flows will be Mb, for streaming Mbps.
	std::vector<double> _TxTime;										//Airtime occupancy of the flow in each interface.
	std::vector<double> _Fc;												//Frequencies in which the flow is Tx in.
	std::vector<std::vector<double>> _Sat;


public:
	Flow();
	~Flow();

	//Setters
	void setType(std::string);
	void setSender(int);
	void setDestination(int);
	void setTimeStamp(double);
	void setTxTime(double);
	void setLength(double);
	void setFc(double);
	void setSat(std::vector<double> &);

	//Getters
	std::string getType();
	int getSender();
	int getDestination();
	double getTimeStamp();
	double getLength();
	std::vector<double> getTxTime();
	std::vector<double> getFc();

	//Other flow methods
	void UpdateFc(std::vector<double> &, std::vector<double> &);
	void UpdateTxTime(std::vector<double> &);
	double getDratio();
	double getSatisfaction();
};
