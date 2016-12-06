#pragma once

#include "BeamLearner.h"
#include <map>
#include <fstream>

namespace SparCraft
{

class Context {
private:
	int numUnitsAlly;
	int numUnitsEnemy;
	int beam;
public:
	void build(GameState & state, int beam, int max_player)
	{
		this->numUnitsAlly = state.numUnits(max_player);
		this->numUnitsEnemy = state.numUnits(state.getEnemy(max_player));
		this->beam = beam;
	}

	Context()
	{
		this->numUnitsAlly = 0;
		this->numUnitsEnemy = 0;
		this->beam = 0;
	}

	void print() const
	{
		std::cout << numUnitsAlly << ", " << numUnitsEnemy << ", " << beam << ", ";
	}

	int getNumUnitsAlly() const
	{
		return numUnitsAlly;
	}

	int getNumUnitsEnemy() const
	{
		return numUnitsEnemy;
	}

	int getBeam() const
	{
		return beam;
	}

	friend bool operator<(const Context& l, const Context& r)
	{
		if(l.numUnitsAlly != r.numUnitsAlly)
			return l.numUnitsAlly < r.numUnitsAlly;
		if(l.numUnitsEnemy != r.numUnitsEnemy)
			return l.numUnitsAlly < r.numUnitsAlly;
		return l.beam < r.beam;
	}
};

class BeamLearner_EpsilonGreedy : public BeamLearner {
private:
	int beam;
	bool learning;
	Context context;
	std::map<Context, Reward> data;
	std::string filename;

	void printAverageReward();
public:
	BeamLearner_EpsilonGreedy(bool learning, int beam, std::string filename);
	void init(GameState & state, const int max_player);
	int getBeamSize(GameState & state, const int max_player);
	void update(GameState & state, const int max_player);
	void writePolicyFile();
	void reset();
};
}
