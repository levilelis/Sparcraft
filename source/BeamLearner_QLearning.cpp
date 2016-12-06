#include "BeamLearner_QLearning.h"

using namespace SparCraft;

BeamLearner_QLearning::BeamLearner_QLearning()
{
	//TODO read the W-values from a file
	previousQ = 0.0;
	previousEval = 0.0;
	max_units = 0;
	max_ltd2 = 0.0;
	for(int i = 0; i < NUM_FEATURES; i++)
		W.push_back(0.0);
}

void BeamLearner_QLearning::init(GameState & state, const int max_player)
{
	previousEval = state.evalLTD2(max_player);
	//StateEvalScore score = state.evalSimNOKDPS(max_player);
	//previousEval = score.val();
	max_units = state.numUnits(Players::Player_One) + state.numUnits(Players::Player_Two);
	max_ltd2 = (double)state.LTD2(Players::Player_One);

	std::cout << "Initial eval: " << previousEval << " with " << max_units << " and max LTD2: " << max_ltd2 << std::endl;
}


double BeamLearner_QLearning::computeQValue(GameState& state, const int max_player, int action)
{
	std::vector<double> attributes;
	computeFeatures(state, attributes, max_player, action);
	double Q = 0.0;
	for(int i = 0; i < attributes.size(); i++)
	{
//		std::cout << "[" << attributes[i] << ", " << W[i] << "], \t";
		Q += attributes[i] * W[i];
	}
//	std::cout << std::endl << std::endl;
	return Q;
}

double BeamLearner_QLearning::computeBestQVAlueAction(GameState& state, const int max_player)
{
	double bestQ = computeQValue(state, max_player, MAX_BEAM_SIZE);
//	std::cout << "[" << MAX_BEAM_SIZE << ", " << bestQ << "], \t";
	for(int i = MAX_BEAM_SIZE - 1; i > 0; i--)
	{
		double Q = computeQValue(state, max_player, i);
//		std::cout << "[" << i << ", " << Q << "], \t";
		if(Q > bestQ)
			bestQ = Q;
	}
//	std::cout << std::endl << std::endl;
	return bestQ;
}

int BeamLearner_QLearning::computeBestAction(GameState& state, const int max_player)
{
	double bestQ = computeQValue(state, max_player, MAX_BEAM_SIZE);
	int bestAction = MAX_BEAM_SIZE;
//	std::cout << "[" << 1 << ", " << bestQ << "], \t";
	for(int i = MAX_BEAM_SIZE - 1; i > 0; i--)
	{
		double Q = computeQValue(state, max_player, i);
		if(Q > bestQ)
		{
			bestQ = Q;
			bestAction = i;
		}
	}
	return bestAction;
}


void BeamLearner_QLearning::computeFeatures(GameState& state, std::vector<double> & attributes, const int max_player, int action)
{
	attributes.clear();
	attributes.push_back((double)state.LTD2(max_player)/max_ltd2);
	attributes.push_back((double)state.LTD2(state.getEnemy(max_player))/max_ltd2);
	attributes.push_back((double)action/MAX_BEAM_SIZE);
//	attributes.push_back((double)action);
//	attributes.push_back((double)(action*action));
	attributes.push_back((double)(action*action)/(MAX_BEAM_SIZE*MAX_BEAM_SIZE));
	attributes.push_back((double)(action*action*action)/(MAX_BEAM_SIZE*MAX_BEAM_SIZE*MAX_BEAM_SIZE));
	attributes.push_back((double)state.numUnits(max_player));
	attributes.push_back((double)state.numUnits(state.getEnemy(max_player)));
	attributes.push_back(1.0);

}

void BeamLearner_QLearning::update(GameState & state, const int max_player)
{
	double currentEval = state.evalLTD2(max_player);
	//StateEvalScore score = state.evalSimNOKDPS(max_player);
	//double currentEval = score.val();
	double reward = currentEval - previousEval;
//	reward /= (state.numUnits(Players::Player_One) + state.numUnits(Players::Player_Two));
	std::cout << "Reward: " << reward << std::endl;

	previousEval = currentEval;

	//double difference = reward + GAMMA * computeQValue(state, max_player, getBeamSize(state, max_player)) - previousQ;
	double difference = reward + GAMMA * computeBestQVAlueAction(state, max_player) - previousQ;
	int bestAction = computeBestAction(state, max_player);
	std::cout << "Current best action: " << bestAction << std::endl;

//	std::cout << "Reward: " << reward  << "\t"
//			<< "Future: " << computeBestQVAlueAction(state, max_player) << "\t Previous:" << previousQ << std::endl;

//	std::vector<double> attributes;
//	computeFeatures(state, attributes, max_player, bestAction);

	for(int i = 0; i < /*W.size()*/ previous_features.size(); i++)
	{
		std::cout << "Update W: " << ALPHA*difference*previous_features[i] <<
				"\t" << difference << "\t" << previous_features[i]<< std::endl;
		//W[i] = W[i] + ALPHA*difference*attributes[i];
		W[i] = W[i] + ALPHA*difference*previous_features[i];
//		std::cout << "W: " << W[i] << std::endl << std::endl;
	}

//	int a;
//	std::cin >> a;

}


int BeamLearner_QLearning::getBeamSize(GameState & state, const int max_player)
{
	int beamSize;
	double p = ((double)(rand() % 100)) / 100.0;
	if(p <= EPSILON)
	{
		beamSize = ((rand() % MAX_BEAM_SIZE) + 1);
		this->previousQ = computeQValue(state, max_player, beamSize);
	}
	else
	{
		beamSize = computeBestAction(state, max_player);
		this->previousQ = computeBestQVAlueAction(state, max_player);
	}

	computeFeatures(state, previous_features, max_player, beamSize);

//	std::cout << "using beam size: " << beamSize << std::endl;

	return beamSize;
}

void BeamLearner_QLearning::writePolicyFile()
{
	for(int i = 1; i < W.size(); i++)
		std::cout << W[i] << "\t";
	std::cout << std::endl;
}

void BeamLearner_QLearning::reset()
{
	previousQ = 0.0;
	previousEval = 0.0;
}
