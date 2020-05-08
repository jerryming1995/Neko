#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

double *GetConfiguration (int IEEEprotocol, int action){

double *configuration = new double[3];

  switch (IEEEprotocol) {
    case 0:{

      int actions [] = {0,1,2,3,4};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {1,6,11,3,11};
      double fc []  = {2.412,2.437,2.462,2.422,2.462};

      if (action <= 2){
        configuration [0] = 20;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else{
        configuration [0] = 40;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
    }break;

    case 1:{

      /*int actions [] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {36,40,44,48,52,56,60,64,38,46,54,62,42,58};
      double fc []  = {5.18,5.2,5.22,5.24,5.26,5.28,5.3,5.32,5.19,5.23,5.27,5.31,5.21,5.29};*/

      int actions [] = {0,1,2,3,4,5,6};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {36,40,44,48,38,46,42};
      double fc []  = {5.18,5.2,5.22,5.24,5.19,5.23,5.21};

      if (action <= 3){ //if (action <= 7){
        configuration [0] = 20;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if((4 == action) || (action == 5)){ //else if((8 <= action) && (action <= 11)){
        configuration [0] = 40;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if (action == 6){ //else if (12<= action){
        configuration [0] = 80;
        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
    }break;

    case 2:{ //802.11ax --> 5GHz Band

      int actions [] = {0,1,2,3,4,5,6};
      int size = sizeof(actions)/sizeof(int);
      int Channel [] = {36,40,44,48,38,46,42};
      double fc []  = {5.18,5.2,5.22,5.24,5.19,5.23,5.21};

      if (action <= 3){ //if (action <= 7){
        configuration [0] = 20;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if((4 == action) || (action == 5)){ //else if((8 <= action) && (action <= 11)){
        configuration [0] = 40;

        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
      else if (action == 6){ //else if (12<= action){
        configuration [0] = 80;
        for (int i=0; i<size; i++){
          if (action == actions[i]){
            configuration [1] = Channel[i];
            configuration [2] = fc[i];
          }
        }
      }
    }break;
  }
  return configuration;
}

int SelectMCS (double Prx, int protocol){

  int MCS_selected;

  switch (protocol) {
    case 0:{
      if (Prx < -82) {MCS_selected = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
      else if (Prx >= -82 && Prx < -79) {MCS_selected = 0;}
      else if (Prx >= -79 && Prx < -77) {MCS_selected = 1;}
      else if (Prx >= -77 && Prx < -74) {MCS_selected = 2;}
      else if (Prx >= -74 && Prx < -70) {MCS_selected = 3;}
      else if (Prx >= -70 && Prx < -66) {MCS_selected = 4;}
      else if (Prx >= -66 && Prx < -65) {MCS_selected = 5;}
      else if (Prx >= -65 && Prx < -64) {MCS_selected = 6;}
      else if (Prx >= -64 && Prx < -59) {MCS_selected = 7;}
      else if (Prx >= -59) {MCS_selected = 7;}
    }break;

    case 1:{
      if (Prx < -82) {MCS_selected = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
      else if (Prx >= -82 && Prx < -79) {MCS_selected = 0;}
      else if (Prx >= -79 && Prx < -77) {MCS_selected = 1;}
      else if (Prx >= -77 && Prx < -74) {MCS_selected = 2;}
      else if (Prx >= -74 && Prx < -70) {MCS_selected = 3;}
      else if (Prx >= -70 && Prx < -66) {MCS_selected = 4;}
      else if (Prx >= -66 && Prx < -65) {MCS_selected = 5;}
      else if (Prx >= -65 && Prx < -64) {MCS_selected = 6;}
      else if (Prx >= -64 && Prx < -59) {MCS_selected = 7;}
      else if (Prx >= -59 && Prx < -57) {MCS_selected = 8;}
      else if (Prx >= -57) {MCS_selected = 9;}
    }break;

    case 2:{
      if (Prx < -82) {MCS_selected = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
      else if (Prx >= -82 && Prx < -79) {MCS_selected = 0;}
      else if (Prx >= -79 && Prx < -77) {MCS_selected = 1;}
      else if (Prx >= -77 && Prx < -74) {MCS_selected = 2;}
      else if (Prx >= -74 && Prx < -70) {MCS_selected = 3;}
      else if (Prx >= -70 && Prx < -66) {MCS_selected = 4;}
      else if (Prx >= -66 && Prx < -65) {MCS_selected = 5;}
      else if (Prx >= -65 && Prx < -64) {MCS_selected = 6;}
      else if (Prx >= -64 && Prx < -59) {MCS_selected = 7;}
      else if (Prx >= -59 && Prx < -57) {MCS_selected = 8;}
      else if (Prx >= -57 && Prx < -54) {MCS_selected = 9;}
      else if (Prx >= -54 && Prx < -52) {MCS_selected = 10;}
      else if (Prx >= -52) {MCS_selected = 11;}
    }break;
  }

  return MCS_selected;
}

double SetDataRate(double RSSI, int IEEEprotocol, int CH_Bandwidth){

  double data_rate;
  int numDataSubcarriers;

  switch (IEEEprotocol) {
    case 0:{
      switch (CH_Bandwidth) {
        case 20:{
          //Total # of subcarriers: 56, 4 are saved for pilot signals.
          numDataSubcarriers = 52;
        }break;

        case 40:{
          //Total # of subcarriers: 114, but 6 are saved for pilot signals.
          numDataSubcarriers = 108;
        }break;
      }

      int MCS_index = SelectMCS(RSSI, IEEEprotocol);
      int BitsSimbol_Modulation [] = {1,2,2,4,4,6,6,6};  //Array of #bits/simbol --> index MCS number. Sequence [BPSK,QPSK,QPSK,16QAM,16QAM,64QAM,64QAM]
      double codingR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6)}; //Array of #bits/simbol --> index MCS number.
      int Nss = 2;              //Number of spatial streams. Here, 2 spatial streams are considered.

      if (MCS_index == -1){
        data_rate = 0;
      }
      else{
        int modBits = BitsSimbol_Modulation [MCS_index];
        double codingRate = codingR [MCS_index];
        data_rate = (Nss * modBits * codingRate * numDataSubcarriers * 1/Tofdm) /1000000; //Mbps
      }
    }break;

    case 1:{
      switch (CH_Bandwidth) {
        case 20:{
          //Total # of subcarriers: 56, 4 are saved for pilot signals.
          numDataSubcarriers = 52;
        }break;

        case 40:{
          //Total # of subcarriers: 114, but 6 are saved for pilot signals.
          numDataSubcarriers = 108;
        }break;

        case 80:{
          //Total # of subcarriers: 242, but 8 are saved for pilot signals.
          numDataSubcarriers = 234;
        }break;
      }

      int MCS_index = SelectMCS(RSSI, IEEEprotocol);
      int BitsSimbol_Modulation [] = {1,2,2,4,4,6,6,6,8,8};  //Array of #bits/simbol --> index MCS number. Sequence [BPSK,QPSK,QPSK,16QAM,16QAM,64QAM,64QAM,256QAM,256QAM]
      double codingR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6),3/double(4),5/double(6)}; //Array of #bits/simbol --> index MCS number. //Array of #bits/simbol --> index MCS number.
      int Nss = 2;              //Number of spatial streams. Here, 2 spatial streams are considered.

      if (MCS_index == -1){
        data_rate = 0;
      }
      else{
        int modBits = BitsSimbol_Modulation [MCS_index];
        double codingRate = codingR [MCS_index];
        data_rate = (Nss * modBits * codingRate * numDataSubcarriers * 1/Tofdm) /1000000; //Mbps
      }

    }break;

    case 2:{
      switch (CH_Bandwidth) {
        case 20:{
          //Total # of subcarriers: 242, 8 are saved for pilot signals.
          numDataSubcarriers = 234;
        }break;

        case 40:{
          //Total # of subcarriers: 484, but 16 are saved for pilot signals.
          numDataSubcarriers = 468;
        }break;

        case 80:{
          //Total # of subcarriers: 996, but 16 are saved for pilot signals.
          numDataSubcarriers = 980;
        }break;
      }

      int MCS_index = SelectMCS(RSSI, IEEEprotocol);
      int BitsSimbol_Modulation [] = {1,2,2,4,4,6,6,6,8,8,10,10};  //Array of #bits/simbol --> index MCS number. Sequence [BPSK,QPSK,QPSK,16QAM,16QAM,64QAM,64QAM,256QAM,256QAM,1024QAM,1024QAM]
      double codingR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6),3/double(4),5/double(6),5/double(6),5/double(6)}; //Array of #bits/simbol --> index MCS number.
      int Nss = 2;              //Number of spatial streams. Here, 2 spatial streams are considered.

      if (MCS_index == -1){
        data_rate = 0;
      }
      else{
        int modBits = BitsSimbol_Modulation [MCS_index];
        double codingRate = codingR [MCS_index];
        data_rate = (Nss * modBits * codingRate * numDataSubcarriers * 1/Tofdm) /1000000; //Mbps
      }

    }break;
  }

  return data_rate;
}
