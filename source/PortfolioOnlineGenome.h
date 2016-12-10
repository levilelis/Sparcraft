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
    std::map<int, IDType>   _unitScriptMap[2][3];
    std::set<IDType>        _scriptSet[2];
    std::vector<IDType>     _scriptVec[2];
    std::vector<PlayerPtr>  _playerPtrVec[2];
    float _mutateRate;
    StateEvalScore _fitness;
    int _iterator;
    IDType _player;
   
    std::vector<Action>       _allScriptMoves[2][PlayerModels::Size];

    std::vector<Action> & getMoves(const IDType & player, const IDType & actualScript);

    Action & getMove(const IDType & player, const IDType & unitIndex, const IDType & actualScript);

public:

    PortfolioOnlineGenome(const IDType & player, const GameState & state);
    PortfolioOnlineGenome(const IDType & player, const GameState & state, const PortfolioOnlineGenome & p1, std::vector<IDType> & portfolio);
    PortfolioOnlineGenome(const IDType & player, const GameState & state, const PortfolioOnlineGenome & p1, const PortfolioOnlineGenome & p2);

    void calculateMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec);
    void calculateMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec, const int level);
    void calculateNextMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec);
    void setUnitScript(const IDType & player, const int & id, const IDType & script);
    void setUnitScript(const Unit & unit, const IDType & script);
    void setUnitScript(const IDType & player, const int & id, const IDType & script, const int level);
    void setUnitScript(const Unit & unit, const IDType & script, const int level);
    void setFitness(const StateEvalScore & fitness);
    void mutate(const IDType & player, const GameState & state, std::vector<IDType> & portfolio);

    void printScripts(const IDType & player, const GameState & state);
    bool hasMoreMoves();
    void resetIterator();

    const IDType &      getUnitScript(const IDType & player, const int & id) const;
    const IDType &      getUnitScript(const Unit & unit) const;
    const IDType &      getUnitScript(const IDType & player, const int & id, const int level) const;
    const IDType &      getUnitScript(const Unit & unit, const int level) const;
    const IDType &      getScript(const IDType & player, const size_t & index);
    const PlayerPtr &   getPlayerPtr(const IDType & player, const size_t & index);
    const size_t        getNumScripts(const IDType & player) const;
    const StateEvalScore & getFitness() const;

    friend bool operator<(const PortfolioOnlineGenome & l, const PortfolioOnlineGenome & r)
    {
        return l._fitness > r._fitness;
    }
};
}
