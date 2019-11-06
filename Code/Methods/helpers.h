
double GetData (int flow, int initVal, int size, std::vector<double>* s, double Bw){
  double bits;
  switch (flow) {
    case 0:{
      double sat, avg_sat;
      int count = 0;
      sat = 0;
      avg_sat = 0;

      for (int i=0; i<size; i++){
        sat = sat + s->at(initVal);
        count++;
        initVal++;
      }

      avg_sat = sat/count;
      bits = Bw*avg_sat;

      //printf("Avg_ Sat: %f, MBits_sent: %f, Mbits_req: %f\n",avg_sat, bits, Bw);
      break;
    }
    case 1:{
      break;
    }
  }
  return bits;
}

int SearchAction (int action, int numOfarms, std::vector<int>* setOfactions){

  int index, value;

  for (int i=0; i<numOfarms; i++){
    value = setOfactions->at(i);
    if (value == action){
      index = i;
    }
  }
  return index;
}

double GetReward (int currentAction, std::vector<double>* s, std::vector<int>* v, std::vector<double>* tS, double t){

  int times, v_value, size;
  double r, avgReward, lowerBound, upperBound, s_value;

  size = tS->size();
  r = 0;
  times = 0;

  if (t-LearningWindowTime < 0){
    lowerBound = 0;
    upperBound = t;
  }
  else{
    lowerBound = t-LearningWindowTime;
    upperBound = t;
  }

  for (int i=0; i<size; i++){
    if ((lowerBound <= tS->at(i)) && (tS->at(i) < upperBound)){
      v_value = v->at(i);
      if (v_value == currentAction){
        s_value = s->at(i);
        r = r + s_value;
        times++;
      }
    }
  }

  if (r == 0){
    avgReward = 0;
  }
  else{
      avgReward = r/times;
  }
  //printf("avgReward: %f\n", avgReward);
  return avgReward;
}

double GetOccupancy (int currentAction, std::vector<double>* o, std::vector<int>* v, std::vector<double>* tS, double t){

  int times, v_value, size;
  double r, avgOccupancy, lowerBound, upperBound, o_value;

  size = tS->size();
  r = 0;
  times = 0;

  if (t-LearningWindowTime < 0){
    lowerBound = 0;
    upperBound = t;
  }
  else{
    lowerBound = t-LearningWindowTime;
    upperBound = t;
  }

  for (int i=0; i<size; i++){
    if ((lowerBound <= tS->at(i)) && (tS->at(i) < upperBound)){
      v_value = v->at(i);
      if (v_value == currentAction){
        o_value = o->at(i);
        r = r + o_value;
        times++;
      }
    }
  }

  if (r == 0){
    avgOccupancy = 0;
  }
  else{
      avgOccupancy = r/times;
  }
  //printf("avgOcc: %f\n", avgOccupancy);
  return avgOccupancy;
}

int ChannelOverlappingDetector (int protocol, int Ch_1, int Ch_2){

  int Ch_overlapped;

  switch (protocol) {
    case 0:{
      //not implemented yet
    }break;

    case 1:{

      switch (Ch_1) {
        case 36:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 40:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 44:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 48:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        /*case 52:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 54)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 56:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 54)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 60:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 62)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 64:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 62)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;*/
        case 38:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 40)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 46:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 44)||(Ch_2 == 48)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        /*case 54:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 52)||(Ch_2 == 56)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 62:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 60)||(Ch_2 == 64)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;*/
        case 42:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 38)||(Ch_2 == 40)||(Ch_2 == 44)||(Ch_2 == 46)||(Ch_2 == 48)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
      /*  case 58:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 52)||(Ch_2 == 54)||(Ch_2 == 56)||(Ch_2 == 60)||(Ch_2 == 62)||(Ch_2 == 64)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;*/
      }
    }break;

    case 2:{

      switch (Ch_1) {
        case 36:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 40:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 38)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 44:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 48:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 46)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        /*case 52:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 54)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 56:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 54)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 60:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 62)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 64:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 62)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;*/
        case 38:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 40)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 46:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 44)||(Ch_2 == 48)||(Ch_2 == 42)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        /*case 54:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 52)||(Ch_2 == 56)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
        case 62:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 60)||(Ch_2 == 64)||(Ch_2 == 58)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;*/
        case 42:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 36)||(Ch_2 == 38)||(Ch_2 == 40)||(Ch_2 == 44)||(Ch_2 == 46)||(Ch_2 == 48)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;
      /*  case 58:{
          if ((Ch_1 == Ch_2)||(Ch_2 == 52)||(Ch_2 == 54)||(Ch_2 == 56)||(Ch_2 == 60)||(Ch_2 == 62)||(Ch_2 == 64)){
            Ch_overlapped = 1;
          }
          else{
            Ch_overlapped = 0;
          }
        }break;*/
      }
    }break;

  }
  return Ch_overlapped;
}
