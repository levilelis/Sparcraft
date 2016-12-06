#include "BeamLearner_EpsilonGreedy.h"

using namespace SparCraft;

BeamLearner_EpsilonGreedy::BeamLearner_EpsilonGreedy(bool learning, int beam, std::string filename)
{
	this->beam = beam;
	this->learning = learning;
	this->filename = filename;
}

void BeamLearner_EpsilonGreedy::init(GameState & state, const int max_player)
{
//	previous_reward = state.evalLTD2(max_player);
//	std::cout << "Initial reward: " << previous_reward << std::endl;
}

void BeamLearner_EpsilonGreedy::update(GameState & state, const int max_player)
{
	double r = state.evalLTD2(max_player);
	if(r > 0) r = 1;
	else r = 0;

	if(data.find(context) == data.end())
	{
		data[context] = Reward();
	}
	data[context].add(r);

	printf("\nPlayer %d, reward %f, and beam %d. \n", max_player, r, beam);
}


int BeamLearner_EpsilonGreedy::getBeamSize(GameState & state, const int max_player)
{
	if(learning)
	{
		//beam = BEAMS[((rand() % NUM_BEAMS))];
		context.build(state, beam, max_player);
	}
	else
	{
		//TODO
		/*
		double maxReward = rewards[BEAMS[0]].getAverageReward();
		int action = 0;
		for(std::map<int, Reward>::iterator it = rewards.begin(); it != rewards.end(); ++it)
		{
			double r = it->second.getAverageReward();
			if(r > maxReward)
			{
				maxReward = r;
				action = it->first;
			}
		}

		beam = BEAMS[action];
		*/
	}
	/*
	//Logic for defining the size of the beam with epsilon-greedy for the next round
	double p = ((double)(rand() % 100)) / 100.0;
	if(p <= EPSILON)
	{
		current_beam_size = ((rand() % MAX_BEAM_SIZE) + 1);
	}
	else
	{
		double max_reward, current_reward;
		rewards[1].getAverageReward(max_reward);
		int index = 1;
		for(int i = 2; i < MAX_BEAM_SIZE+1; i++)
		{
			rewards[i].getAverageReward(current_reward);
			if(current_reward > max_reward)
			{
				max_reward = current_reward;
				index = i;
			}
		}
		current_beam_size = index;
	}

	current_beam_size = ((rand() % MAX_BEAM_SIZE) + 1);
	*/

	return beam;
}

void BeamLearner_EpsilonGreedy::writePolicyFile()
{
	std::ofstream out;
	out.open(filename.c_str());
	if(!out.is_open())
	{
        System::FatalError("Problem Opening Output File: " + filename);
		return;
	}
	for(std::map<Context, Reward>::iterator it = data.begin(); it != data.end(); ++it)
	{
		out << it->first.getNumUnitsAlly() << ", " << it->first.getNumUnitsEnemy() << ", " << it->first.getBeam() << ", " << it->second.getAverageReward() << "\n";
	}
	out.close();
}

void BeamLearner_EpsilonGreedy::reset()
{
	//this->previous_reward = 0;
}
