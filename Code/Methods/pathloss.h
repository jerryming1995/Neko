

double PropL(double x, double y, double z, double x1, double y1, double z1, double f){

  switch (propagation) {

    //BUILDING SCENARIO -- It does consider floors, z != 1;
    case 0:{
      int n_walls = 5;                                         //Wall frequency [m/wall]
      int n_floors = 2;                                        //Number of floors crossed (in X and Y axis)
      double dBP = 5;                                         //Distance in meters (m).
      double dBP_losses, PL, distance, expo;

      distance = sqrt(pow(x-x1, 2)+pow(y-y1, 2)+pow(z-z1, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      expo=((distance/n_floors)+2)/((distance/n_floors)+1)-0.46;

      PL = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 18.3*pow((distance/n_floors),expo)+ 5*(distance/n_walls);

      return PL;

    }break;

    //ENTERPRISE SCENARIO -- It does not consider floors, z = 1;
    case 1:{

      int n_walls = 4;                                          //Wall frequency [m/wall]
      double dBP = 5;                                           //Distance in meters (m).
      double dBP_losses, PL, distance;

      distance = sqrt(pow(x-x1, 2)+pow(y-y1, 2)+pow(z-z1, 2));

      if (distance >= dBP){
        dBP_losses = 35*log10(distance/dBP);
      }
      else{
        dBP_losses = 0;
      }

      PL = 40.05 + 20*log10(f/2.4) + 20*log10(std::min(distance,dBP)) + dBP_losses + 7*n_walls;

      return PL;

    }break;

    //TMB pathloss -- https://arxiv.org/abs/1812.00667
    case 2:{



    }break;
  }
}

double Dmax(double sensitivity, double TxPower, double f){

  switch (propagation){
    case 1:{
        int n_walls = 4;                                         //Wall frequency [m/wall]
        double dBP = 5;                                         //Distance in meters (m).

        double ChW = GetChWFromFc(f);
        double nCh = ChW/20;
        double PtxLineal = pow(10,((TxPower-30)/10));
        double Ptx_dBm = 10*log10(PtxLineal/nCh) + 30;

        double PL = Ptx_dBm - sensitivity;
        double distance = dBP*pow(10,((PL - 40.05 - 20*log10(f/2.4) - 20*log10(dBP) - 7*n_walls)/35));

        return distance;
    }
  }
}
