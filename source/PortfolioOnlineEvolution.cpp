#include "PortfolioOnlineEvolution.h"

using namespace SparCraft;

PortfolioOnlineEvolution::PortfolioOnlineEvolution(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit)
	: _player(player)
	, _enemyScript(enemyScript)
	, _iterations(iter)
    , _responses(responses)
    , _totalEvals(0)
    , _timeLimit(timeLimit)
	, _populationSize(16)
	, _playoutLimit(25)
	, _selectedMembers(4)
	, _offspringPerSelected(3)
{
	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);
	_playerScriptPortfolio.push_back(PlayerModels::Cluster);
//	_playerScriptPortfolio.push_back(PlayerModels::MoveForward);
//	_playerScriptPortfolio.push_back(PlayerModels::MoveBackward);

	srand(1234);
}

void PortfolioOnlineEvolution::init(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	for(int i = 0; i < this->_populationSize; i++)
	{
		PortfolioOnlineGenome gen(player, state);
		population.push_back(gen);
	}
}

void PortfolioOnlineEvolution::mutatePopulation(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	std::vector<PortfolioOnlineGenome>  newPopulation;
	//std::cout << "Population size before mutation: " << population.size() << " selected: " << _selectedMembers << std::endl;
	for(int j = 0; j < population.size(); j++)
	{
		//population[j].mutate(player, state, _playerScriptPortfolio);
		for(int i = 0; i < _offspringPerSelected; i++)
		{
			PortfolioOnlineGenome offspring(player, state, population[j], _playerScriptPortfolio);
			newPopulation.push_back(offspring);
		}
	}
	for(int i = 0; i < newPopulation.size(); i++)
	{
		population.push_back(newPopulation[i]);
	}
//	std::cout << "Population size after mutation: " << population.size() << std::endl << std::endl;
}

void PortfolioOnlineEvolution::select(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	std::vector<PortfolioOnlineGenome> newPopulation;
	for(int i = 0; i < _selectedMembers; i++)
	{
		newPopulation.push_back(population[i]);
	}
	population.clear();
	population = newPopulation;
}

void PortfolioOnlineEvolution::crossover(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	std::vector<PortfolioOnlineGenome> newPopulation;
	for(int i = 0; i < _selectedMembers; i++)
	{
		newPopulation.push_back(population[i]);
	}

	for(int i = 0; i < _selectedMembers; i++)
	{
		for(int j = 0; j < _selectedMembers; j++)
		{
			if(i == j)
			{
				continue;
			}

			PortfolioOnlineGenome offspring(player, state, population[rand() % population.size()], population[rand() % population.size()]);
			offspring.mutate(player, state, _playerScriptPortfolio);
			newPopulation.push_back(offspring);
		}
	}
	population.clear();
	population = newPopulation;
}

void PortfolioOnlineEvolution::evalPopulation(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population)
{
	const IDType enemyPlayer(state.getEnemy(player));
	for(int i = 0; i < population.size(); i++)
	{
		Game g(state, 100);
		_totalEvals++;
		population[i].setFitness(g.playoutGenome(player, population[i], _playoutLimit));
	}

	std::sort(population.begin(), population.end());
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
    	evalPopulation(player, state, population);
    	select(player, state, population);
    //	mutatePopulation(player, state, population);
    	crossover(player, state, population);

    	ms = t.getElapsedTimeInMilliSec();
    }

	evalPopulation(player, state, population);

//    ms = t.getElapsedTimeInMilliSec();
//    printf("\nMove POE chosen in %lf ms\n", ms);

    // convert the script vector into a move vector and return it
	MoveArray moves;
	state.generateMoves(moves, player);
    std::vector<Action> moveVec;
    GameState copy(state);
    population[0].calculateMoves(player, moves, copy, moveVec);

    _totalEvals = 0;

    return moveVec;
}

