#pragma once

#include "Common.h"
#include "GameState.h"
#include "MoveArray.h"
#include "Unit.h"
#include <memory>

namespace SparCraft
{

class Reward
{
	double reward;
	int n;
public:
	Reward()
	{
		n = 0;
		reward = 0.0;
	}
	double getAverageReward()
	{
		if(n == 0)
		{
			return 0.0;
		}
		return reward/n;
	}
	void add(double r)
	{
		reward += r;
		n++;
	}
};

class BeamLearner
{
protected:
	const int MAX_BEAM_SIZE = 60;

public:
    virtual int			getBeamSize(GameState & state, int max_player);
    virtual void 		update(GameState & state, int max_player);
    virtual void		writePolicyFile();
    virtual void		reset();
    virtual void 		init(GameState & state, const int max_player);
};

}
