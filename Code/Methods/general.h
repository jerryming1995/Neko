
double CalculateTxTime(double L, double TxRate){

	double TimeMPDU, Tack, Trts, Tcts, LDBPS_DL, AirTime;

  LDBPS_DL = (TxRate*pow(10,6))*Tofdm;

	TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+Lpckt+Ltb)/(LDBPS_DL)))*Tofdm;
	Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
	Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
	Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

	AirTime = (std::ceil((L*pow(10,6)/Lpckt))*(1/(1-Pe)))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty));

	return AirTime;
}

double GetFromTxTime(double at, double TxRate){

	double TimeMPDU, Tack, Trts, Tcts, LDBPS_DL, load;

  LDBPS_DL = (TxRate*pow(10,6))*Tofdm;

	TimeMPDU = TphyHE + std::ceil(((Lsf+Lmac+Lpckt+Ltb)/(LDBPS_DL)))*Tofdm;
	Tack = TphyL + std::ceil(((Lsf+Lack+Ltb)/(legacyRate)))*Tofdm_leg;
	Trts = TphyL + std::ceil(((Lsf+Lrts+Ltb)/(legacyRate)))*Tofdm_leg;
	Tcts = TphyL + std::ceil(((Lsf+Lcts+Ltb)/(legacyRate)))*Tofdm_leg;

	load = (at*Lpckt)/((1/(1-Pe))*(((CW/2)*Tempty)+(Trts+Tsifs+Tcts+Tsifs+TimeMPDU+Tsifs+Tack+Tdifs+Tempty))*pow(10,6));
	return load;
}

double CalculateSNR(double RSSI, double ChW){

	double NoiseFloor = -174 + NF /*Noise Figure*/ + 10*log10(ChW*1E6);
	double SNR = RSSI - NoiseFloor;

	return SNR;
}

double CalculateRSSI(double Ptx, double fc, double x, double y, double z, double x1, double y1, double z1){

	/*Since channels may be higher than 20MHz, we need to calculate the
	power over the whole set of subcarriers. Thus, reducing it -3dB.

	Ex: ChW = 80; so, nCh = 80/20 = 4 --> PtxL/4*/
	double ChW = GetChWFromFc(fc);
	double nCh = ChW/20;
	double PtxLineal = pow(10,((Ptx-30)/10));
	double Ptx_dBm = 10*log10(PtxLineal/nCh) + 30;

	double RSSI = Ptx_dBm - PropL(x, y, z, x1, y1, z1, fc);

	return RSSI;
}

double CalculateDataRate(double SNR, double fc, double ChW, Capabilities &cap, Configuration &config){

	int MCS_index = GetMCS(SNR, cap.IEEEProtocol, ChW);
	double rate;
	if (MCS_index >= 0){

		int BitsPerSimbol = GetBitsPerSimbol(MCS_index);
		int Subcarriers = GetSubcarriers(fc, ChW);
		double CodingR = GetCodingRate(MCS_index);

		rate = (config.nSS * BitsPerSimbol * CodingR * Subcarriers * 1/Tofdm) /1000000; //Mbps
	}
	else{
		rate = 0;
		//std::cout<<"ERROR: station out of CCA. Data rate set to 0Mbps"<<std::endl;
	}

	return rate;
}

double GetActReward (int action, std::vector<int> *action_selection, std::vector<double> *received_reward, std::vector<double> *t_action, double simt){

  int size = t_action->size();
  int times = 0;

  double lowerBound = std::max(0.0,simt-WinTime);
  double upperBound = simt;
	double sumReward = 0;
	double avgReward = 0;

  for (int i=0; i<size; i++){
    if ((lowerBound <= t_action->at(i)) && (t_action->at(i) < upperBound)){
      if (action_selection->at(i) == action){
        sumReward += received_reward->at(i);
        times++;
      }
    }
  }

  if (sumReward != 0)
      avgReward = sumReward/times;
  //printf("avgReward: %f\n", avgReward);
  return avgReward;
}
