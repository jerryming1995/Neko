
int GetMCS (double SNR, int protocol, int ChW){

	int MCS;
	switch (protocol) {

		/*802.11ax*/
      	case 1:{
      		switch (ChW){
      			case 20:{
      				if (SNR < 2) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 2 && SNR < 5) {MCS = 0;}
					else if (SNR >= 5 && SNR < 9) {MCS = 1;}
					else if (SNR >= 9 && SNR < 11) {MCS = 2;}
					else if (SNR >= 11 && SNR < 15) {MCS = 3;}
					else if (SNR >= 15 && SNR < 18) {MCS = 4;}
					else if (SNR >= 18 && SNR < 20) {MCS = 5;}
					else if (SNR >= 20 && SNR < 25) {MCS = 6;}
					else if (SNR >= 25 && SNR < 29) {MCS = 7;}
					else if (SNR >= 29 && SNR < 31) {MCS = 8;}
					else if (SNR >= 31 && SNR < 34) {MCS = 9;}
					else if (SNR >= 34 && SNR < 37) {MCS = 10;}
					else if (SNR >= 37) {MCS = 11;}
      			}break;
      			case 40:{
      				if (SNR < 5) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 5 && SNR < 8) {MCS = 0;}
					else if (SNR >= 8 && SNR < 12) {MCS = 1;}
					else if (SNR >= 12 && SNR < 14) {MCS = 2;}
					else if (SNR >= 14 && SNR < 18) {MCS = 3;}
					else if (SNR >= 18 && SNR < 21) {MCS = 4;}
					else if (SNR >= 21 && SNR < 23) {MCS = 5;}
					else if (SNR >= 23 && SNR < 28) {MCS = 6;}
					else if (SNR >= 28 && SNR < 32) {MCS = 7;}
					else if (SNR >= 32 && SNR < 34) {MCS = 8;}
					else if (SNR >= 34 && SNR < 37) {MCS = 9;}
					else if (SNR >= 37 && SNR < 40) {MCS = 10;}
					else if (SNR >= 40) {MCS = 11;}
      			}break;
      			case 80:{
      				if (SNR < 8) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 8 && SNR < 11) {MCS = 0;}
					else if (SNR >= 11 && SNR < 15) {MCS = 1;}
					else if (SNR >= 15 && SNR < 17) {MCS = 2;}
					else if (SNR >= 17 && SNR < 21) {MCS = 3;}
					else if (SNR >= 21 && SNR < 24) {MCS = 4;}
					else if (SNR >= 24 && SNR < 26) {MCS = 5;}
					else if (SNR >= 26 && SNR < 31) {MCS = 6;}
					else if (SNR >= 31 && SNR < 35) {MCS = 7;}
					else if (SNR >= 35 && SNR < 37) {MCS = 8;}
					else if (SNR >= 37 && SNR < 40) {MCS = 9;}
					else if (SNR >= 40 && SNR < 42) {MCS = 10;}
					else if (SNR >= 42) {MCS = 11;}
      			}break;
      		}
      	}break;

      	/*802.11be*/
      	case 2:{
      		switch (ChW){
      			case 20:{
      				if (SNR < 2) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 2 && SNR < 5) {MCS = 0;}
					else if (SNR >= 5 && SNR < 9) {MCS = 1;}
					else if (SNR >= 9 && SNR < 11) {MCS = 2;}
					else if (SNR >= 11 && SNR < 15) {MCS = 3;}
					else if (SNR >= 15 && SNR < 18) {MCS = 4;}
					else if (SNR >= 18 && SNR < 20) {MCS = 5;}
					else if (SNR >= 20 && SNR < 25) {MCS = 6;}
					else if (SNR >= 25 && SNR < 29) {MCS = 7;}
					else if (SNR >= 29 && SNR < 31) {MCS = 8;}
					else if (SNR >= 31 && SNR < 34) {MCS = 9;}
					else if (SNR >= 34 && SNR < 37) {MCS = 10;}
					else if (SNR >= 37) {MCS = 11;}
      			}break;
      			case 40:{
      				if (SNR < 5) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 5 && SNR < 8) {MCS = 0;}
					else if (SNR >= 8 && SNR < 12) {MCS = 1;}
					else if (SNR >= 12 && SNR < 14) {MCS = 2;}
					else if (SNR >= 14 && SNR < 18) {MCS = 3;}
					else if (SNR >= 18 && SNR < 21) {MCS = 4;}
					else if (SNR >= 21 && SNR < 23) {MCS = 5;}
					else if (SNR >= 23 && SNR < 28) {MCS = 6;}
					else if (SNR >= 28 && SNR < 32) {MCS = 7;}
					else if (SNR >= 32 && SNR < 34) {MCS = 8;}
					else if (SNR >= 34 && SNR < 37) {MCS = 9;}
					else if (SNR >= 37 && SNR < 40) {MCS = 10;}
					else if (SNR >= 40) {MCS = 11;}
      			}break;
      			case 80:{
      				if (SNR < 8) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 8 && SNR < 11) {MCS = 0;}
					else if (SNR >= 11 && SNR < 15) {MCS = 1;}
					else if (SNR >= 15 && SNR < 17) {MCS = 2;}
					else if (SNR >= 17 && SNR < 21) {MCS = 3;}
					else if (SNR >= 21 && SNR < 24) {MCS = 4;}
					else if (SNR >= 24 && SNR < 26) {MCS = 5;}
					else if (SNR >= 26 && SNR < 31) {MCS = 6;}
					else if (SNR >= 31 && SNR < 35) {MCS = 7;}
					else if (SNR >= 35 && SNR < 37) {MCS = 8;}
					else if (SNR >= 37 && SNR < 40) {MCS = 9;}
					else if (SNR >= 40 && SNR < 42) {MCS = 10;}
					else if (SNR >= 42) {MCS = 11;}
      			}break;
      			case 160:{
      				if (SNR < 11) {MCS = -1;} //Delimiter. Invalid number of MCS, so it reflects an exception to indicate that a station can't be connected to an AP because its outside the range.
					else if (SNR >= 11 && SNR < 14) {MCS = 0;}
					else if (SNR >= 14 && SNR < 18) {MCS = 1;}
					else if (SNR >= 18 && SNR < 20) {MCS = 2;}
					else if (SNR >= 20 && SNR < 24) {MCS = 3;}
					else if (SNR >= 24 && SNR < 27) {MCS = 4;}
					else if (SNR >= 27 && SNR < 29) {MCS = 5;}
					else if (SNR >= 29 && SNR < 34) {MCS = 6;}
					else if (SNR >= 34 && SNR < 38) {MCS = 7;}
					else if (SNR >= 38 && SNR < 40) {MCS = 8;}
					else if (SNR >= 40 && SNR < 42) {MCS = 9;}
					else if (SNR >= 42 && SNR < 44) {MCS = 10;}
					else if (SNR >= 44) {MCS = 11;}
      			}break;
      		}
      	}break;
    }
    return MCS;
}

int GetBitsPerSimbol(int index){

	int BitsSimbol[] = {1,2,2,4,4,6,6,6,8,8,10,10};  //Array of #bits/simbol --> index MCS number. Sequence [BPSK,QPSK,QPSK,16QAM,16QAM,64QAM,64QAM,64QAM,256QAM,256QAM,1024QAM,1024QAM]
	return BitsSimbol[index];
}

double GetCodingRate(int index){

	double CR [] = {1/double(2),1/double(2),3/double(4),1/double(2),3/double(4),2/double(3),3/double(4),5/double(6),3/double(4),5/double(6),3/double(4),5/double(6)}; //Array of #bits/simbol --> index MCS number.
	return CR[index];
}

int GetSubcarriers(double fc, int width){

	int subcarriers;
	int band = (int)fc;

	switch (band){
		case 2:{
			switch (width){
				case 20:{subcarriers = 234;}break;
				case 40:{subcarriers = 468;}break;
			}
		}break;

		case 5:{
			switch (width){
				case 20:{subcarriers = 234;}break;
				case 40:{subcarriers = 468;}break;
				case 80:{subcarriers = 980;}break;
			}
		}break;

		case 6:{
			switch (width){
				case 20:{subcarriers = 234;}break;
				case 40:{subcarriers = 468;}break;
				case 80:{subcarriers = 980;}break;
				case 160:{subcarriers = 1960;}break;
			}
		}break;
	}
	return subcarriers;
}
