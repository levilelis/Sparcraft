#include "PortfolioOnlineEvolution.h"

using namespace SparCraft;

PortfolioOnlineEvolution::PortfolioOnlineEvolution(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit)
	: _player(player)
	, _enemyScript(enemyScript)
	, _iterations(iter)
    , _responses(responses)
    , _totalEvals(0)
    , _timeLimit(timeLimit)
	, _populationSize(30)
	, _playoutLimit(25)
{
	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);
	_playerScriptPortfolio.push_back(PlayerModels::Cluster);
}

void PortfolioOnlineEvolution::init(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	for(int i = 0; i < this->_populationSize; i++)
	{
		PortfolioOnlineGenome gen(state);
		population.push_back(gen);
	}
}

void PortfolioOnlineEvolution::mutatePopulation(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	std::vector<PortfolioOnlineGenome>::iterator gen_it = population.begin();
	for(; gen_it != population.end(); ++gen_it)
	{
		gen_it->mutate(player, state, this->_playerScriptPortfolio);
	}
}

std::vector<Action> PortfolioOnlineEvolution::search(const IDType & player, const GameState & state)
{
    Timer t;
    t.start();

    const IDType enemyPlayer(state.getEnemy(player));

    double ms = t.getElapsedTimeInMilliSec();
    //printf("\nFirst Part %lf ms\n", ms);

    std::vector<PortfolioOnlineGenome> population;
    init(player, state, population);

    while(ms < this->_timeLimit)
    {
    	mutatePopulation(player, state, population);
    	evalPopulation(player, state, population);

    	ms = t.getElapsedTimeInMilliSec();
    }

    // convert the script vector into a move vector and return it
	MoveArray moves;
	state.generateMoves(moves, player);
    std::vector<Action> moveVec;
    GameState copy(state);
    population[0].calculateMoves(player, moves, copy, moveVec);

    ms = t.getElapsedTimeInMilliSec();
    //printf("\nMove PGS chosen in %lf ms\n", ms);

    _totalEvals = 0;

    return moveVec;
}
/*
void PortfolioOnlineEvolution::doPortfolioSearch(const IDType & player, const GameState & state, PortfolioOnlineGenome & currentScriptData)
{
    Timer t;
    t.start();

    // the enemy of this player
    const IDType enemyPlayer(state.getEnemy(player));
    
    for (size_t i(0); i<_iterations; ++i)
    {
        // set up data for best scripts
        IDType          bestScriptVec[Constants::Max_Units];
	    StateEvalScore  bestScoreVec[Constants::Max_Units];

        // for each unit that can move
        for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
        {
            if (_timeLimit > 0 && t.getElapsedTimeInMilliSec() > _timeLimit)
            {
                break;
            }

            const Unit & unit(state.getUnit(player, unitIndex));

            // iterate over each script move that it can execute
            for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
            {
                // set the current script for this unit
                currentScriptData.setUnitScript(unit, _playerScriptPortfolio[sIndex]);

                // evaluate the current state given a playout with these unit scripts
                StateEvalScore score = eval(player, state, currentScriptData);

                // if we have a better score, set it
                if (sIndex == 0 || score > bestScoreVec[unitIndex])
                {
                    bestScriptVec[unitIndex] = _playerScriptPortfolio[sIndex];
                    bestScoreVec[unitIndex]  = score;
                }
            }

            // set the current vector to the best move for use in future simulations
            currentScriptData.setUnitScript(unit, bestScriptVec[unitIndex]);
        }
    }   
}
*/
/*
IDType PortfolioOnlineEvolution::calculateInitialSeed(const IDType & player, const GameState & state)
{
    IDType bestScript;
    StateEvalScore bestScriptScore;
    const IDType enemyPlayer(state.getEnemy(player));
    
    // try each script in the portfolio for each unit as an initial seed
    for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
    {
    	PortfolioOnlineGenome currentScriptData;
    
        // set the player's chosen script initially to the seed choice
        for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
        {
            currentScriptData.setUnitScript(state.getUnit(player, unitIndex), _playerScriptPortfolio[sIndex]);
        }

        // set the enemy units script choice to NOKDPS
        for (size_t unitIndex(0); unitIndex < state.numUnits(enemyPlayer); ++unitIndex)
        {
            currentScriptData.setUnitScript(state.getUnit(enemyPlayer, unitIndex), _enemyScript);
        }

        // evaluate the current state given a playout with these unit scripts
        StateEvalScore score = eval(player, state, currentScriptData);

        if (sIndex == 0 || score > bestScriptScore)
        {
            bestScriptScore = score;
            bestScript = _playerScriptPortfolio[sIndex];
        }
    }

    return bestScript;
}*/

void PortfolioOnlineEvolution::evalPopulation(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	const IDType enemyPlayer(state.getEnemy(player));

	for(int i = 0; i < population.size(); i++)
	{
		Game g(state, 100);
		_totalEvals++;
		population[i].setFitness(g.playoutGenome(player, population[i], 3));
	}

	std::sort(population.begin(), population.end());

//	std::cout << "Printing population" << std::endl;
//	for(int i = 0; i < population.size(); i++)
//	{
//		std::cout << population[i].getFitness().val() << "\t";
//	}
//	std::cout << std::endl;
}
