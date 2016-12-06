/*
#include "PortfolioOnlineGenome2.h"

#include "Random.hpp"

using namespace SparCraft;

PortfolioOnlineGenome::PortfolioOnlineGenome(const IDType & player, const GameState & state)
	: _mutateRate(0.2),
	  _rand(0, std::numeric_limits<int>::max(), Constants::Seed_Player_Random_Time ? static_cast<unsigned int>(std::time(0)) : 0),
	  _fitness(StateEvalScore())
{
	this->_player = player;

	//initializing the script of all units for all levels with NO-OVERKILL
	for(int level = 0; level < 3; level++)
	{
		for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
		{
			setUnitScript(level, state.getUnit(player, unitIndex), PlayerModels::NOKDPS);
		}
	}
}

void PortfolioOnlineGenome::mutate(const IDType & player, const GameState & state, std::vector<IDType> & portfolio)
{
	for(int level = 0; level < 3; level++)
	{
		for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
		{
			if(_rand.nextInt() % 100 < (this->_mutateRate * 100))
			{
				int replace = _rand.nextInt() % portfolio.size();
				setUnitScript(level, state.getUnit(player, unitIndex), portfolio[replace]);
			}
		}
	}
}

std::vector<Action> & PortfolioOnlineGenome::getMoves(const IDType actualScript)
{
    return _allScriptMoves[actualScript];
}

Action & PortfolioOnlineGenome::getMove(const IDType unitIndex, const IDType actualScript)
{
    return _allScriptMoves[actualScript][unitIndex];
}

void PortfolioOnlineGenome::calculateMoves(MoveArray & moves, GameState & state, std::vector<Action> & moveVec, int level)
{
    // generate all script moves for this player at this state and store them in allScriptMoves
    for (size_t scriptIndex(0); scriptIndex<_scriptVec.size(); ++scriptIndex)
    {
        // get the associated player pointer
        const PlayerPtr & pp = getPlayerPtr(scriptIndex);

        // get the actual script we are working with
        const IDType actualScript = getScript(scriptIndex);

        // generate the moves inside the appropriate vector
        getMoves(actualScript).clear();
        pp->getMoves(state, moves, getMoves(actualScript));
    }

    // for each unit the player has to move, populate the move vector with the appropriate script move
    for (size_t unitIndex(0); unitIndex < moves.numUnits(); ++unitIndex)
    {
        // the unit from the state
        const Unit & unit = state.getUnit(_player, unitIndex);

        // the move it would choose to do based on its associated script preference
        Action unitMove = getMove(unitIndex, getUnitScript(level, unit));

        // put the unit into the move vector
        moveVec.push_back(unitMove);
    }
}

const IDType PortfolioOnlineGenome::getUnitScript(const int & level, const int & id) const
{
    return (*_unitScriptMap[level].find(id)).second;
}
    
const IDType PortfolioOnlineGenome::getUnitScript(const int & level, const Unit & unit) const
{
    return getUnitScript(level, unit.ID());
}

const IDType & PortfolioOnlineGenome::getScript(const size_t & index)
{
    return _scriptVec[index];
}

const PlayerPtr & PortfolioOnlineGenome::getPlayerPtr(const size_t & index)
{
    return _playerPtrVec[index];
}

const size_t PortfolioOnlineGenome::getNumScripts() const
{
    return _scriptSet.size();
}

void PortfolioOnlineGenome::setUnitScript(const int & level, const int & id, const IDType & script)
{
    if (_scriptSet.find(script) == _scriptSet.end())
    {
        _scriptSet.insert(script);
        _scriptVec.push_back(script);
        _playerPtrVec.push_back(PlayerPtr(AllPlayers::getPlayerPtr(this->_player, script)));
    }

    _unitScriptMap[level][id] = script;
}

void PortfolioOnlineGenome::setFitness(const StateEvalScore & fitness)
{
	this->_fitness = fitness;
}

const StateEvalScore & PortfolioOnlineGenome::getFitness() const
{
	return this->_fitness;
}

void PortfolioOnlineGenome::setUnitScript(const int & level, const Unit & unit, const IDType & script)
{
    setUnitScript(level, unit.ID(), script);
}*/
