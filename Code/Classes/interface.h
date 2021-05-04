
class Interface{

public:

  int id;											//Interface id;
	int ChN;										//Channel number.
	int ChW;										//Channel width.
	double fc;										//Central frequency
};

// --------------------------------------------------------------------------------------------------------------------------------------------------//
class APInterface : public Interface{

public:
  double TOcc;
  double ChOccSFlows;								//Channel occupancy by own traffic (streaming flows)
  double ChOccNeighAPs;							//Channel occupancy due to neighboring Aps

public:
  APInterface();
  ~APInterface();
};

APInterface::APInterface(){
  TOcc = 0;
  ChOccSFlows = 0;
  ChOccNeighAPs = 0;
}

APInterface::~APInterface(){
  
}

// --------------------------------------------------------------------------------------------------------------------------------------------------//
class STAInterface : public Interface{

public:
  bool active;									/*Wheter and interface is active or not. (True or false). This parameter is established by the MLO Setup negotiation*/
};
