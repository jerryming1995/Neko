#include <math.h>
#include <random>

//        --------- Epsilon greedy --------        //

int Egreedy(int num_actions, std::vector<double>* reward_per_arm, std::vector<double>* occupancy, double epsilon, std::vector<int>* times_arm_selected) {

	double rndProbability = ((double) rand() / (RAND_MAX));
	int arm_index;
	double r_value, o_value;

	if (rndProbability < epsilon) {
		arm_index = rand() % num_actions;
		printf("exploring\n");
	}
	else {

		printf("exploiting\n");
		double max_rew = 0;
		double min_occ = 100;
		double th = 0.85;

		for (int i = 0; i < num_actions; i ++){
			r_value = reward_per_arm->at(i);
			o_value = occupancy->at(i);

			if (r_value >= th){
				if(o_value <= min_occ){
					max_rew = r_value;
					min_occ = o_value;
					arm_index = i;
				}
			}
			else{
				if(r_value >= max_rew){
					max_rew = r_value;
					min_occ = o_value;
					arm_index = i;
				}
			}
		}
	}
	times_arm_selected->at(arm_index) = times_arm_selected->at(arm_index) + 1;
	return arm_index;
}

//        --------- Thompson Sampling --------        //

double gaussrand(double mean, double std){

	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0) {
		do {
			double U1 = (double)rand() /  RAND_MAX;
			double U2 = (double)rand() /  RAND_MAX;
			V1 = 2*U1 - 1;
			V2 = 2*U2 - 1;
			S = V1 * V1 + V2 * V2;
		}
    while (S >= 1 || S == 0);
		X = (V1 * sqrt(-2 * log(S) / S)) * std + mean;
	}
  else {
		X = (V1 * sqrt(-2 * log(S) / S)) * std + mean;
	}
	phase = 1 - phase;

	return X;
}


int ThompsonSampling (int num_actions, std::vector<double>* estimated_reward, std::vector<double>* occupancy, std::vector<int>* times_arm_selected) {

	int action_ix = 0;
  double theta[num_actions] = {0};
	double mean, variance, times;

	for (int i = 0; i < num_actions; i++) {

		times = times_arm_selected->at(i);
		mean = estimated_reward->at(i);
		variance = 1/(1+times);
		theta[i] = gaussrand(mean,variance);
	}

	double max = 0;
	for (int i = 0; i < num_actions; i ++) {
		if(theta[i] > max) {
			max = theta[i];
			action_ix = i;
		}
	}

	times_arm_selected->at(action_ix) = times_arm_selected->at(action_ix) + 1;
	return action_ix;
}
