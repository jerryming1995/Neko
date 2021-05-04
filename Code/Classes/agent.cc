
#include "agent.h"

//Intended to be used by APs as their action set is either the Channels vector or the MLO policies
void Agent::setActSpace(){

  if (_type.compare("AP") == 0){
    ActionSpace = Channels;
    for (int i=0; i<(int)ActionSpace.size(); i++){
      std::vector<double> v((int)ActionSpace.at(i).size(), 0.0);
      Rewards.push_back(v);
      TimesActSel.push_back(v);
      EstRewardAct.push_back(v);
    }
  }
}

//Intended to be used by stations as they must pass the vector of APs that they have in range
/*void Agent::setActSpace(std::vector<int> &v){

}*/

std::string Agent::getTarget(){
  return _target;
}

//Intended to be used when using MLO as the MLO policy is passed along the act-reward vector of pairs
std::vector<int> Agent::getNewAction(std::vector<std::pair<int,double>> &ActRew_vector){

  std::vector<int> new_actions;
  for (int i=0; i<(int)ActRew_vector.size(); i++){
    std::pair<int,double> p = ActRew_vector.at(i);
    for (int j=0; j<(int)ActionSpace.size(); j++){
      for (int n=0; n<(int)ActionSpace.at(i).size(); n++){
        if (ActionSpace[j].at(n) == p.first){
          TimesActSel[j].at(n) += 1;
          Rewards[j].at(n) = p.second;
          EstRewardAct[j].at(n) = ((EstRewardAct[j].at(n)*TimesActSel[j].at(n))+Rewards[j].at(n))/(TimesActSel[j].at(n)+2);
          int action_arm = ThompsonSampling(ActionSpace.at(j).size(), &EstRewardAct.at(j), &TimesActSel.at(j));
          new_actions.push_back(ActionSpace[j].at(action_arm));
          //std::cout << "**LAST ACTION: " << ActionSpace[j].at(n) << " " << Rewards[j].at(n) << " " << TimesActSel[j].at(n) << " " << EstRewardAct[j].at(n) << " NEWACT: " << ActionSpace[j].at(action_arm) << std::endl;
        }
      }
    }
  }

  return new_actions;
}
