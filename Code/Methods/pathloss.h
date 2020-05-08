#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>


double PropL(double clientX, double clientY, double clientZ, double ApX, double ApY, double ApZ, double f){

  switch (propagation) {

    //BUILDING SCENARIO -- It does consider floors, z != 1;
    case 0:{
      int n_walls = 5;                                         //Wall frequency [m/wall]
      int n_floors = 2;                                        //Number of floors crossed (in X and Y axis)
      double dBP = 5;                                         //Distance in meters (m).
      double expo;
      double dBP_losses, propagationLosses;
      double distance;                                          //Current distance from one client to the AP/relay.

      distance = sqrt(pow(ApX-clientX, 2)+pow(ApY-clientY, 2)+pow(ApZ-clientZ, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      expo=((distance/n_floors)+2)/((distance/n_floors)+1)-0.46;

      propagationLosses = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 18.3*pow((distance/n_floors),expo)+ 5*(distance/n_walls);

      return propagationLosses;

    }break;

    //ENTERPRISE SCENARIO -- It does not consider floors, z = 1;
    case 1:{

      int n_walls = 4;                                         //Wall frequency [m/wall]
      double dBP = 5;                                         //Distance in meters (m).
      double dBP_losses, propagationLosses;
      double distance;                                         //Current distance from one client to the AP/relay.

      distance = sqrt(pow(ApX-clientX, 2)+pow(ApY-clientY, 2)+pow(ApZ-clientZ, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      propagationLosses = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 7*n_walls;

      return propagationLosses;

    }break;

    //TMB pathloss -- https://arxiv.org/abs/1812.00667
    case 2:{



    }break;
  }
}
