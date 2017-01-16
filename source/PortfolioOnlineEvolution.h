#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include <memory>
#include <algorithm>
#include <ctime>
#include "PortfolioOnlineGenome.h"

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;

class PortfolioOnlineEvolution
{
protected:
	
    const IDType				_player;
    const IDType				_enemyScript;
    const size_t				_iterations;
    const size_t                _responses;
    std::vector<IDType>			_playerScriptPortfolio;
    size_t                      _totalEvals;
    size_t                      _timeLimit;
    size_t						_populationSize;
    size_t						_playoutLimit;
    size_t						_selectedMembers;
    size_t						_offspringPerSelected;
    std::ofstream				_fileTime;

    void						init(const IDType & player,const GameState & state, std::vector<PortfolioOnlineGenome> & population);
    void                        doPortfolioSearch(const IDType & player,const GameState & state,PortfolioOnlineGenome & currentData);
    void 						mutatePopulation(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population);
    void						crossover(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population);
    void						select(const IDType & player, const GameState & state, std::vector<PortfolioOnlineGenome> & population);
    std::vector<Action>     	getMoveVec(const IDType & player,const GameState & state,const std::vector<IDType> & playerScripts);
    void              			evalPopulation(const IDType & player,const GameState & state, std::vector<PortfolioOnlineGenome> & population);
    IDType                      calculateInitialSeed(const IDType & player,const GameState & state);

public:

    PortfolioOnlineEvolution(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit);
    std::vector<Action> search(const IDType & player, const GameState & state);
};

}
