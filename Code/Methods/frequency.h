
std::string GetBand (double fc){

	std::string band;
	int fc_band = (int)fc;

	switch (fc_band){
		case 2:{band = "2_4GHz";}break;
		case 5:{band = "5GHz";}break;
		case 6:{band = "6GHz";}break;
	}
	return band;
}

std::pair<double,int> GetFromChN(int ChN){

	double fc;
	int ChW;

	if (ChN == 1){fc = 2.412; ChW = 20;}
	else if( ChN == 6){fc = 2.437; ChW = 20;}
	else if( ChN == 11){fc = 2.462; ChW = 20;}
	else if( ChN == 3){fc = 2.422; ChW = 40;}
	else if( ChN == 36){fc = 5.18; ChW = 20;}
	else if( ChN == 38){fc = 5.19; ChW = 40;}
	else if( ChN == 46){fc = 5.23; ChW = 40;}
	else if( ChN == 42){fc = 5.21; ChW = 80;}
	else if( ChN == 58){fc = 5.29; ChW = 80;}
	else if( ChN == 25){fc = 6.065; ChW = 20;}
	else if( ChN == 51){fc = 6.195; ChW = 40;}
	else if( ChN == 55){fc = 6.215; ChW = 80;}
	else if( ChN == 71){fc = 6.295; ChW = 80;}
	else if( ChN == 47){fc = 6.175; ChW = 160;}
	else if( ChN == 15){fc = 6.015; ChW = 160;}

	return std::make_pair(fc,ChW);
}

bool CheckNeighChOverlapp(double fc, double neighFc){

	bool overlap = false;
	int fc_band = (int)fc;

	switch (fc_band){
		//In the 2.4GHz band, we set available 4 channels --> (20MHz) 1,6 and 11 // (40Mhz) 3
		case 2:{
			if((fc == neighFc) || ((fc == 2.412) && (neighFc == 2.422)) || ((fc == 2.422) && (neighFc == 2.412))){
				overlap = true;
			}
		}break;

		//In the 5GHz band, we set available 4 channels --> (20MHz) 36 // (40Mhz) 38 and 46 // (80) 42
		case 5:{
			if ((fc == neighFc) || ((fc == 5.18) && (neighFc == 5.19)) || ((fc == 5.19) && (neighFc == 5.18)) || ((fc == 5.18) && (neighFc == 5.21))|| ((fc == 5.21) && (neighFc == 5.18)) ||
																									((fc == 5.19) && (neighFc == 5.21)) || ((fc == 5.21) && (neighFc == 5.19)) || ((fc == 5.23) && (neighFc == 5.21)) || ((fc == 5.21) && (neighFc == 5.23))){
				overlap = true;
			}
		}break;
		//In the 6GHz band, we set available 4 channels --> (40Mhz) 51// (80) 55 and 71 // (160) 47
		case 6:{
			if ((fc == neighFc) || ((fc == 6.195) && (neighFc == 6.175)) || ((fc == 6.175) && (neighFc == 6.195)) || ((fc == 6.195) && (neighFc == 6.215)) ||
																							((fc == 6.215) && (neighFc == 6.195)) || ((fc == 6.215) && (neighFc == 6.175)) || ((fc == 6.175) && (neighFc == 6.215))){
				overlap = true;
			}
		}break;
	}
	return overlap;
}

bool CheckChOverlapp(int ch1, int ch2){

	std::pair<double, int> p = GetFromChN(ch1);
	int band = (int)p.first;
	bool overlap = false;

	switch (band){

		case 2:{
			if((ch1 == ch2) || ((ch1 == 1) && (ch2 == 3)) || ((ch1 == 3) && (ch2 == 1))){
				overlap = true;
			}
		}break;

		case 5:{
			if((ch1 == ch2) || ((ch1 == 36) && (ch2 == 38)) || ((ch1 == 38) && (ch2 == 36)) || ((ch1 == 36) && (ch2 == 42)) || ((ch1 == 42) && (ch2 == 36))
																															|| ((ch1 == 38) && (ch2 == 42)) || ((ch1 == 42) && (ch2 == 38)) || ((ch1 == 46) && (ch2 == 42)) || ((ch1 == 42) && (ch2 == 46))){
				overlap = true;
			}
		}break;

		case 6:{
			if((ch1 == ch2) || ((ch1 == 51) && (ch2 == 55)) || ((ch1 == 55) && (ch2 == 51)) || ((ch1 == 51) && (ch2 == 47))
																											|| ((ch1 == 47) && (ch2 == 51)) || ((ch1 == 55) && (ch2 == 47)) || ((ch1 == 47) && (ch2 == 55))){
				overlap = true;
			}
		}break;
	}

	return overlap;
}

int GetChWFromFc (double fc){

	int ChW;
	int fc_band = (int)fc;

	switch (fc_band){
		case 2:{
			if (fc == 2.422){ChW = 40;}
			else{ChW = 20;}
		}break;

		case 5:{
			if (fc == 5.18){ChW = 20;}
			else if ((fc == 5.19) || (fc == 5.23)){ChW = 40;}
			else{ChW = 80;}
		}break;

		case 6:{
			if (fc == 6.065){ChW = 20;}
			else if (fc == 6.195){ChW = 40;}
			else if ((fc == 6.215) || (fc == 6.295)){ChW = 80;}
			else{ChW = 160;}
		}break;
	}
	return ChW;
}
