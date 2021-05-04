
class Agent{

public:
  Agent(std::string type, std::string trgt, std::string func):_type(type), _target(trgt), _OptFunct(func){};

  std::string _type;                                                    //Type of agent (AP or STA)
  std::string _target;                                                  //Target goal of the agent (DAPS or DCA)
  std::string _OptFunct;                                                //Optimization function (Egreedy or TSampling)

  std::vector<std::vector<double>> ActionSpace;                         //Action space for the agent. Matrix (arms x actions per arm), for APs it will be the channel number for each Channel, and STAs the sensed APs.
  std::vector<std::vector<double>> Rewards;                             //Historic of rewards for each action
  std::vector<std::vector<double>> EstRewardAct;                        //Estimated reward per action
  std::vector<std::vector<double>> TimesActSel;                         //Times an action has been selected

  //Setters
  void setActSpace();
  //void setActSpace(std::vector<int> &v);                              //For stations, which must pass the detected APs above the CCA.

  //Getters
  std::string getTarget();
  std::vector<int> getNewAction(std::vector<std::pair<int,double>> &v);   //Function to select the new parameters (APs, ChN or MLO policy). We pass the pair action-reward for all the available interfaces
};
