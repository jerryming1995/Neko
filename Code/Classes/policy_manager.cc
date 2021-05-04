
#include "policy_manager.h"

PolicyManager::PolicyManager(){

}

void PolicyManager::setType(std::string type){
  _policy = type;
}

std::string PolicyManager::getType(){
  return _policy;
}

void PolicyManager::AllocationFromPolicyEqual(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){
  for (int i=0; i<(int)AssociatedSTAs.size(); i++){
    if (flow->getDestination() == AssociatedSTAs.at(i).id){
      int sta_interfaces = (int)AssociatedSTAs.at(i).fc.size();
      double fLength = flow->getLength();
      double LChunk = fLength/sta_interfaces;
      for (int j=0; j<(int)AssociatedSTAs.at(i).TxRate.size(); j++){
        flow->setTxTime(CalculateTxTime(LChunk, AssociatedSTAs.at(i).TxRate.at(j)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(j));
      }
      break;
    }
  }
}

void PolicyManager::AllocationFromPolicyOne(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){

  double occ_max = 100;
  int interface_index = -1;
  for (int i=0; i<(int)AssociatedSTAs.size(); i++){
    if (flow->getDestination() == AssociatedSTAs.at(i).id){
      if ((int)AssociatedSTAs.at(i).fc.size() > 1){
        for (int j=0; j<(int)AssociatedSTAs.at(i).fc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (AssociatedSTAs.at(i).fc.at(j) == InterfaceContainer.at(n).fc){
              if (InterfaceContainer.at(n).TOcc < occ_max) {
                occ_max = InterfaceContainer.at(n).TOcc;
                interface_index = j;
              }
            }
          }
        }
        double fLength = flow->getLength();
        flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(interface_index)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(interface_index));
      }
      else{
        for (int n=0; n<(int)InterfaceContainer.size(); n++){
          if (AssociatedSTAs.at(i).fc.at(0) == InterfaceContainer.at(n).fc){
            interface_index = InterfaceContainer.at(n).id;
          }
        }
        double fLength = flow->getLength();
        flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(0)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(0));
      }
      break;
    }
  }
}

void PolicyManager::AllocationFromPolicyFixed(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){

  int sta_index;
  for (int i=0; i<(int)InterfaceContainer.size(); i++){
    if (flow != nullptr){
      for (int j=0; j<(int)AssociatedSTAs.size(); j++){
        if (AssociatedSTAs.at(j).id == flow->getDestination()){
          sta_index = j;
          for (int n=0; n<(int)AssociatedSTAs.at(j).fc.size(); n++){
            if (AssociatedSTAs.at(j).fc.at(n) == InterfaceContainer.at(i).fc){
              flow->setFc(AssociatedSTAs.at(j).fc.at(n));
            }
          }
          break;
        }
      }
    }
  }

  double fLength = flow->getLength();
  std::vector<double> FlowFc = flow->getFc();
  std::vector<double> allocated_load((int)FlowFc.size(), 0.0);
  std::vector<double> interface_id, empty_load;

  while (fLength > 1E-3){
    empty_load.assign((int)FlowFc.size(), 0.0);
    interface_id.clear();
    double length = fLength;
    if (FlowFc.size() > 1){
      for (int j=0; j<(int)FlowFc.size(); j++){
        for (int n=0; n<(int)InterfaceContainer.size(); n++){
          if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
            if (InterfaceContainer.at(n).TOcc + allocated_load.at(j) < 1){
              interface_id.push_back(InterfaceContainer.at(n).id);
              empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
              break;
            }
          }
        }
      }

      if (interface_id.size() == 0){
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
              for (int m=0; m<(int)FlowFc.size(); m++){
                for (int l=0; l<(int)InterfaceContainer.size(); l++){
                  if (FlowFc.at(m) == InterfaceContainer.at(l).fc){
                    if ((std::floor(InterfaceContainer.at(l).TOcc + allocated_load.at(m)) == std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j))) && (InterfaceContainer.at(l).id != InterfaceContainer.at(n).id)){
                      if (interface_id.size() != 0){
                        bool found = false;
                        for (int f=0; f<(int)interface_id.size(); f++){
                          if (interface_id.at(f) == InterfaceContainer.at(n).id){
                            found = true;
                            break;
                          }
                        }
                        if (!found){
                          interface_id.push_back(InterfaceContainer.at(n).id);
                          empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                        }
                      }
                      else{
                        interface_id.push_back(InterfaceContainer.at(n).id);
                        empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                      }
                    }
                    break;
                  }
                }
              }
              break;
            }
          }
        }
      }
      if (interface_id.size() != 0){
        double empty_total = std::accumulate(empty_load.begin(), empty_load.end(), 0.0);
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if ((FlowFc.at(j) == InterfaceContainer.at(n).fc)){
              double prop = empty_load.at(j)/empty_total;
              for (int m=0; m<(int)AssociatedSTAs.at(sta_index).fc.size(); m++){
                if (AssociatedSTAs.at(sta_index).fc.at(m) == FlowFc.at(j)){
                  double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(m))*prop;
                  if ((std::ceil(InterfaceContainer.at(n).TOcc) == 0) || (InterfaceContainer.at(n).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(n).TOcc+ allocated_load.at(j)))){
                    allocated_load.at(j) += tmp_load;
                    fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                  }
                  else if ((InterfaceContainer.at(n).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(n).TOcc + allocated_load.at(j))){
                    allocated_load.at(j) += tmp_load;
                    fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                  }
                  else{
                    double time_left = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                    if (time_left < tmp_load){
                      allocated_load.at(j) += time_left;
                      fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                    else{
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                  }
                  break;
                }
              }
              break;
            }
          }
        }
      }
      else{
        double load = 100;
        int interface = -1;
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if ((FlowFc.at(j) == InterfaceContainer.at(n).fc) && ((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) < load)){
              load = InterfaceContainer.at(n).TOcc + allocated_load.at(j);
              interface = n;
              break;
            }
          }
        }
        for (int j=0; j<(int)FlowFc.size(); j++){
          if (InterfaceContainer.at(interface).fc == FlowFc.at(j)){
            for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
              if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
                double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
                if ((std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) == 0) || (InterfaceContainer.at(interface).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(interface).TOcc+ allocated_load.at(j)))){
                  allocated_load.at(j) += tmp_load;
                  fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                }
                else if ((InterfaceContainer.at(interface).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j))){
                  allocated_load.at(j) += tmp_load;
                  fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                }
                else{
                  double time_left = 1-((InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)));
                  if (time_left < tmp_load){
                    allocated_load.at(j) += time_left;
                    fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                  else{
                    allocated_load.at(j) += tmp_load;
                    fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                }
                break;
              }
            }
            break;
          }
        }
      }
    }
    else{
      for (int j=0; j<(int)FlowFc.size(); j++){
        for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
          if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
            allocated_load.at(j) = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
            fLength = 0;
          }
        }
      }
    }
  }

  for (int i=0; i<(int)allocated_load.size(); i++){
    flow->setTxTime(allocated_load.at(i));
  }
}

std::pair<bool,std::vector<double>> PolicyManager::AllocationFromPolicyAdaptive(Flow *flow, std::vector<Flow> &OnGoingFlows, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer, double t){

  std::vector<Flow> ActiveFlows = OnGoingFlows;
  std::vector<double> InterfaceOcc, InterfaceFc;

  for (int i=0; i<(int)InterfaceContainer.size(); i++){
    InterfaceOcc.push_back(InterfaceContainer.at(i).ChOccSFlows);
    InterfaceFc.push_back(InterfaceContainer.at(i).fc);
    InterfaceContainer.at(i).TOcc -= InterfaceContainer.at(i).ChOccSFlows;
    if (InterfaceContainer.at(i).TOcc < 1E-10)
      InterfaceContainer.at(i).TOcc = 0;
    InterfaceContainer.at(i).ChOccSFlows = 0;
    if (flow != nullptr){
      for (int j=0; j<(int)AssociatedSTAs.size(); j++){
        if (AssociatedSTAs.at(j).id == flow->getDestination()){
          for (int n=0; n<(int)AssociatedSTAs.at(j).fc.size(); n++){
            if (AssociatedSTAs.at(j).fc.at(n) == InterfaceContainer.at(i).fc){
              flow->setFc(AssociatedSTAs.at(j).fc.at(n));
            }
          }
          break;
        }
      }
    }
  }

  if (flow != nullptr){
    ActiveFlows.push_back(*flow);
  }

  std::sort(ActiveFlows.begin(), ActiveFlows.end(), [](Flow& lhs, Flow &rhs){
    return lhs.getFc().size() < rhs.getFc().size();
  });

  for (int i=0; i<(int)ActiveFlows.size();i++){

    double fLength = ActiveFlows.at(i).getLength();
    std::vector<double> FlowFc = ActiveFlows.at(i).getFc();
    std::vector<double> allocated_load((int)FlowFc.size(), 0.0);
    std::vector<double> interface_id, empty_load;

    double sta_index;
    for (int j=0; j<(int)AssociatedSTAs.size(); j++){
      if (AssociatedSTAs.at(j).id == ActiveFlows.at(i).getDestination()){
        sta_index = j;
        break;
      }
    }

    while (fLength > 1E-3){
      empty_load.assign((int)FlowFc.size(), 0.0);
      interface_id.clear();
      double length = fLength;
      if (FlowFc.size() > 1){
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
              if (InterfaceContainer.at(n).TOcc + allocated_load.at(j) < 1){
                interface_id.push_back(InterfaceContainer.at(n).id);
                empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
              }
              break;
            }
          }
        }
        if (interface_id.size() == 0){
          for (int j=0; j<(int)FlowFc.size(); j++){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
                for (int m=0; m<(int)FlowFc.size(); m++){
                  for (int l=0; l<(int)InterfaceContainer.size(); l++){
                    if (FlowFc.at(m) == InterfaceContainer.at(l).fc){
                      if (InterfaceContainer.at(n).TOcc + allocated_load.at(j) < 1){
                        interface_id.push_back(InterfaceContainer.at(n).id);
                        empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                      }

                      else if ((std::floor(InterfaceContainer.at(l).TOcc + allocated_load.at(m)) == std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j))) && (InterfaceContainer.at(l).id != InterfaceContainer.at(n).id)){
                        if (interface_id.size() != 0){
                          bool found = false;
                          for (int f=0; f<(int)interface_id.size(); f++){
                            if (interface_id.at(f) == InterfaceContainer.at(n).id){
                              found = true;
                              break;
                            }
                          }
                          if (!found){
                            interface_id.push_back(InterfaceContainer.at(n).id);
                            empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                          }
                        }
                        else{
                          interface_id.push_back(InterfaceContainer.at(n).id);
                          empty_load.at(j) = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                        }
                      }
                      break;
                    }
                  }
                }
                break;
              }
            }
          }
        }
        if (interface_id.size() != 0){
          double empty_total = std::accumulate(empty_load.begin(), empty_load.end(), 0.0);
          for (int j=0; j<(int)FlowFc.size(); j++){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if ((FlowFc.at(j) == InterfaceContainer.at(n).fc)){
                double prop = empty_load.at(j)/empty_total;
                for (int m=0; m<(int)AssociatedSTAs.at(sta_index).fc.size(); m++){
                  if (AssociatedSTAs.at(sta_index).fc.at(m) == FlowFc.at(j)){
                    double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(m))*prop;
                    if ((std::ceil(InterfaceContainer.at(n).TOcc) == 0) || (InterfaceContainer.at(n).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(n).TOcc+ allocated_load.at(j)))){
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                    else if ((InterfaceContainer.at(n).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(n).TOcc + allocated_load.at(j))){
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                    }
                    else{
                      double time_left = 1-((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(n).TOcc + allocated_load.at(j)));
                      if (time_left < tmp_load){
                        allocated_load.at(j) += time_left;
                        fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(m));
                      }
                      else{
                        allocated_load.at(j) += tmp_load;
                        fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(m));
                      }
                    }
                    break;
                  }
                }
                break;
              }
            }
          }
        }
        else{
          double load = 100;
          int interface = -1;
          for (int j=0; j<(int)FlowFc.size(); j++){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if ((FlowFc.at(j) == InterfaceContainer.at(n).fc) && ((InterfaceContainer.at(n).TOcc + allocated_load.at(j)) < load)){
                load = InterfaceContainer.at(n).TOcc + allocated_load.at(j);
                interface = n;
                break;
              }
            }
          }
          for (int j=0; j<(int)FlowFc.size(); j++){
            if (InterfaceContainer.at(interface).fc == FlowFc.at(j)){
              for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
                if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
                  double tmp_load = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  if ((std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) == 0) || (InterfaceContainer.at(interface).TOcc + allocated_load.at(j) == std::ceil(InterfaceContainer.at(interface).TOcc+ allocated_load.at(j)))){
                     allocated_load.at(j) += tmp_load;
                     fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                  else if ((InterfaceContainer.at(interface).TOcc + allocated_load.at(j) + tmp_load) < std::ceil(InterfaceContainer.at(interface).TOcc + allocated_load.at(j))){
                     allocated_load.at(j) += tmp_load;
                     fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                  }
                  else{
                    double time_left = 1-((InterfaceContainer.at(interface).TOcc + allocated_load.at(j)) - std::floor(InterfaceContainer.at(interface).TOcc + allocated_load.at(j)));
                    if (time_left < tmp_load){
                      allocated_load.at(j) += time_left;
                      fLength -= GetFromTxTime(time_left, AssociatedSTAs.at(sta_index).TxRate.at(n));
                    }
                    else{
                      allocated_load.at(j) += tmp_load;
                      fLength -= GetFromTxTime(tmp_load, AssociatedSTAs.at(sta_index).TxRate.at(n));
                    }
                  }
                  break;
                }
              }
              break;
            }
          }
        }
      }
      else{
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)AssociatedSTAs.at(sta_index).fc.size(); n++){
            if (AssociatedSTAs.at(sta_index).fc.at(n) == FlowFc.at(j)){
              allocated_load.at(j) = CalculateTxTime(length, AssociatedSTAs.at(sta_index).TxRate.at(n));
              fLength = 0;
            }
          }
        }
      }
    }

    if (flow != nullptr){
      if (ActiveFlows.at(i).getDestination() == flow->getDestination()){
        for (int j=0; j<(int)allocated_load.size(); j++){
          flow->setTxTime(allocated_load.at(j));
        }
      }
      else{
        for (int j=0; j<(int)FlowFc.size(); j++){
          for (int n=0; n<(int)InterfaceContainer.size(); n++){
            if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
              InterfaceContainer.at(n).ChOccSFlows += allocated_load.at(j);
              InterfaceContainer.at(n).TOcc += allocated_load.at(j);
            }
          }
        }

        for (int j=0; j<(int)OnGoingFlows.size(); j++){
          if (OnGoingFlows.at(j).getDestination() == ActiveFlows.at(i).getDestination()){
            OnGoingFlows.at(j).UpdateTxTime(allocated_load);
            break;
          }
        }
      }
    }
    else{
      for (int j=0; j<(int)FlowFc.size(); j++){
        for (int n=0; n<(int)InterfaceContainer.size(); n++){
          if (FlowFc.at(j) == InterfaceContainer.at(n).fc){
            InterfaceContainer.at(n).ChOccSFlows += allocated_load.at(j);
            InterfaceContainer.at(n).TOcc += allocated_load.at(j);
          }
        }
      }
      for (int j=0; j<(int)OnGoingFlows.size(); j++){
        if (OnGoingFlows.at(j).getDestination() == ActiveFlows.at(i).getDestination()){
          OnGoingFlows.at(j).UpdateTxTime(allocated_load);
          break;
        }
      }
    }
  }
  bool change = false;
  for (int j=0; j<(int)InterfaceOcc.size(); j++){
    InterfaceOcc.at(j) = InterfaceContainer.at(j).ChOccSFlows-InterfaceOcc.at(j);
    if ((InterfaceOcc.at(j) != 0) && (change == false))
      change = true;
  }
  return std::make_pair(change, InterfaceOcc);
}

void PolicyManager::AllocationFromPolicySplit(Flow *flow, std::vector<WifiSTA> &AssociatedSTAs, std::vector<APInterface> &InterfaceContainer){

  std::string type = flow->getType();
  if (type.compare("STREAMING") == 0){
    for (int i=0; i<(int)AssociatedSTAs.size(); i++){
      if (flow->getDestination() == AssociatedSTAs.at(i).id){
        for (int j=0; j<(int)AssociatedSTAs.at(i).fc.size(); j++){
          std::string band = GetBand(AssociatedSTAs.at(i).fc.at(j));
          if (band.compare("6GHz") == 0){
            double fLength = flow->getLength();
            flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(j)));
            flow->setFc(AssociatedSTAs.at(i).fc.at(j));
            break; //Only allocate to the first 6GHz band, do not expect to have more than one 6GHz interface
          }
        }
      }
    }
  }
  else{
    double occ_max = 100;
    int interface_index = -1;
    for (int i=0; i<(int)AssociatedSTAs.size(); i++){
      if (flow->getDestination() == AssociatedSTAs.at(i).id){
        for (int j=0; j<(int)AssociatedSTAs.at(i).fc.size(); j++){
          std::string band = GetBand(AssociatedSTAs.at(i).fc.at(j));
          if (band.compare("6GHz") != 0){
            for (int n=0; n<(int)InterfaceContainer.size(); n++){
              if (AssociatedSTAs.at(i).fc.at(j) == InterfaceContainer.at(n).fc){
                if (InterfaceContainer.at(n).TOcc < occ_max) {
                  occ_max = InterfaceContainer.at(n).TOcc;
                  interface_index = j;
                }
                break;
              }
            }
          }
        }
        double fLength = flow->getLength();
        flow->setTxTime(CalculateTxTime(fLength, AssociatedSTAs.at(i).TxRate.at(interface_index)));
        flow->setFc(AssociatedSTAs.at(i).fc.at(interface_index));
      }
    }
  }
}
