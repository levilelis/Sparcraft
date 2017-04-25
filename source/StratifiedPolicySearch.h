#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include "UnitScriptData.h"
#include <memory>
#include "StratType.h"
#include <fstream>

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;

class StratifiedPolicySearch
{
protected:
	
    const IDType				_player;
    const IDType				_enemyScript;
    const size_t				_iterations;
    const size_t                _responses;
    std::vector<IDType>			_playerScriptPortfolio;
    size_t                      _totalEvals;
    size_t                      _timeLimit;
    size_t						_hpLevelDiv;
    std::ofstream				_fileTime;

    void                        doStratifiedSearch(const IDType & player,const GameState & state,UnitScriptData & currentData, Timer & timer);
    std::vector<Action>     	getMoveVec(const IDType & player,const GameState & state,const std::vector<IDType> & playerScripts);
    StateEvalScore              eval(const IDType & player,const GameState & state,UnitScriptData & playerScriptsChosen);
    IDType                      calculateInitialSeed(const IDType & player,const GameState & state);
    void                        setAllScripts(const IDType & player,const GameState & state,UnitScriptData & data,const IDType & script);

public:

    StratifiedPolicySearch(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit, const size_t & hpLevelDiv);
    std::vector<Action> search(const IDType & player, const GameState & state);
};

}
