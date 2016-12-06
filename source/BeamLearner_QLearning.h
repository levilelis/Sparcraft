#pragma once

#include <iostream>
#include "BeamLearner.h"

namespace SparCraft
{
class BeamLearner_QLearning : public BeamLearner {

private:
	const double EPSILON = 0.2;
	const double GAMMA = 0.9;
	const double ALPHA = 0.01;
	const int NUM_FEATURES = 8;
	std::vector<double> W;
	std::vector<double> previous_features;
	double previousQ;
	double previousEval;
	int max_units;
	double max_ltd2;

	void printAverageReward();
	void computeFeatures(GameState& state, std::vector<double> & attributes, const int max_player, int action);
	double computeQValue(GameState& state, const int max_player, int action);
	double computeBestQVAlueAction(GameState& state, const int max_player);
	int computeBestAction(GameState& state, const int max_player);
public:
	BeamLearner_QLearning();
    int getBeamSize(GameState & state, const int max_player);
    void update(GameState & state, const int max_player);
    void init(GameState & state, const int max_player);
    void writePolicyFile();
    void reset();

};
}
