/*
#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "AllPlayers.h"
#include "Action.h"
#include <memory>
#include <set>

namespace SparCraft
{
	
typedef	std::shared_ptr<Player> PlayerPtr;


class PortfolioOnlineGenome
{
    // map from UnitID to PlayerModel
    std::map<int, int>   _unitScriptMap[3];
    std::set<IDType>        _scriptSet;
    std::vector<IDType>     _scriptVec;
    std::vector<PlayerPtr>  _playerPtrVec;
    IDType _player;
    float _mutateRate;
    StateEvalScore _fitness;
    RandomInt _rand;
   
    std::vector<Action>       _allScriptMoves[PlayerModels::Size];
    std::vector<Action> & getMoves(const IDType actualScript);
    Action & getMove(const IDType unitIndex, const IDType actualScript);

public:

    PortfolioOnlineGenome(const IDType & type, const GameState & state);

    void calculateMoves(MoveArray & moves, GameState & state, std::vector<Action> & moveVec, int level = 0);
    void setUnitScript(const int & level, const int & id, const IDType & script);
    void setUnitScript(const int & level, const Unit & unit, const IDType & script);
    void setFitness(const StateEvalScore & fitness);
    void mutate(const IDType & player, const GameState & state, std::vector<IDType> & portfolio);

    const IDType        getUnitScript(const int & level, const int & id) const;
    const IDType        getUnitScript(const int & level, const Unit & unit) const;
    const IDType &      getScript(const size_t & index);
    const PlayerPtr &   getPlayerPtr(const size_t & index);
    const size_t        getNumScripts() const;
    const StateEvalScore & getFitness() const;

    friend bool operator<(const PortfolioOnlineGenome & l, const PortfolioOnlineGenome & r)
    {
        return l._fitness > r._fitness;
    }
};

}*/
