#include "PortfolioOnlineGenome.h"

using namespace SparCraft;

PortfolioOnlineGenome::PortfolioOnlineGenome(const IDType & player, const GameState & state)
: _mutateRate(0.2),
  _fitness(StateEvalScore()),
  _iterator(0)
{
	_player = player;
	IDType players[2] = {Players::Player_One, Players::Player_Two};
	//initializing the script of all units for both players with NO-OVERKILL
	for(int p = 0; p < 2; p++)
	{
		for(int level = 0; level < 3; level++)
		{
			for (size_t unitIndex(0); unitIndex < state.numUnits(players[p]); ++unitIndex)
			{
				setUnitScript(state.getUnit(players[p], unitIndex), PlayerModels::NOKDPS, level);
			}
		}
	}
}

PortfolioOnlineGenome::PortfolioOnlineGenome(const IDType & player, const GameState & state, const PortfolioOnlineGenome & p1, const PortfolioOnlineGenome & p2)
: _mutateRate(0.2),
  _fitness(StateEvalScore()),
  _iterator(0)
{
	_player = player;
	//initializing the script of all units for both players with NO-OVERKILL

	IDType players[2] = {Players::Player_One, Players::Player_Two};
	//initializing the script of all units for both players with NO-OVERKILL
	for(int p = 0; p < 2; p++)
	{
		for(int level = 0; level < 3; level++)
		{
			for (size_t unitIndex(0); unitIndex < state.numUnits(players[p]); ++unitIndex)
			{
				//50% of chance of getting a gene from p1 or p2
				if(rand() % 2)
				{
					IDType script(p1.getUnitScript(state.getUnit(players[p], unitIndex), level));
					setUnitScript(state.getUnit(players[p], unitIndex), script, level);
				}
				else
				{
					IDType script(p2.getUnitScript(state.getUnit(players[p], unitIndex), level));
					setUnitScript(state.getUnit(players[p], unitIndex), script, level);
				}
			}
		}
	}
}

PortfolioOnlineGenome::PortfolioOnlineGenome(const IDType & player, const GameState & state, const PortfolioOnlineGenome & p1, std::vector<IDType> & portfolio)
: _mutateRate(0.2),
  _fitness(StateEvalScore()),
  _iterator(0)
{
	_player = player;
	//initializing the script of all units for both players with NO-OVERKILL

	IDType players[2] = {Players::Player_One, Players::Player_Two};
	//initializing the script of all units for both players with NO-OVERKILL
	for(int p = 0; p < 2; p++)
	{
		for(int level = 0; level < 3; level++)
		{
			for (size_t unitIndex(0); unitIndex < state.numUnits(players[p]); ++unitIndex)
			{
				if(players[p] == player && rand() % 100 < (this->_mutateRate * 100))
				{
					int replace = rand() % portfolio.size();
					setUnitScript(state.getUnit(player, unitIndex), portfolio[replace], level);
				}
				else
				{
					IDType script(p1.getUnitScript(state.getUnit(players[p], unitIndex), level));
					//std::cout << "Script: " << (int) script << std::endl;
					setUnitScript(state.getUnit(players[p], unitIndex), script, level);
				}
			}
		}
	}
}


void PortfolioOnlineGenome::mutate(const IDType & player, const GameState & state, std::vector<IDType> & portfolio)
{
	for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
	{
		for(int level = 0; level < 3; level++)
		{
			if(rand() % 100 < (this->_mutateRate * 100))
			{
				int replace = rand() % portfolio.size();
				setUnitScript(state.getUnit(player, unitIndex), portfolio[replace], level);
			}
		}
	}
}

std::vector<Action> & PortfolioOnlineGenome::getMoves(const IDType & player, const IDType & actualScript)
{
	return _allScriptMoves[player][actualScript];
}

Action & PortfolioOnlineGenome::getMove(const IDType & player, const IDType & unitIndex, const IDType & actualScript)
{
	return _allScriptMoves[player][actualScript][unitIndex];
}

void PortfolioOnlineGenome::calculateMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec, const int level)
{
	// generate all script moves for this player at this state and store them in allScriptMoves
	for (size_t scriptIndex(0); scriptIndex<_scriptVec[player].size(); ++scriptIndex)
	{
		// get the associated player pointer
		const PlayerPtr & pp = getPlayerPtr(player, scriptIndex);

		// get the actual script we are working with
		const IDType actualScript = getScript(player, scriptIndex);

		// generate the moves inside the appropriate vector
		getMoves(player, actualScript).clear();
		pp->getMoves(state, moves, getMoves(player, actualScript));
	}

	// for each unit the player has to move, populate the move vector with the appropriate script move
	for (size_t unitIndex(0); unitIndex < moves.numUnits(); ++unitIndex)
	{
		// the unit from the state
		const Unit & unit = state.getUnit(player, unitIndex);

		// the move it would choose to do based on its associated script preference
		Action unitMove = getMove(player, unitIndex, getUnitScript(unit, level));

		// put the unit into the move vector
		moveVec.push_back(unitMove);
	}
}

void PortfolioOnlineGenome::resetIterator()
{
	_iterator = 0;
}

bool PortfolioOnlineGenome::hasMoreMoves()
{
	return _iterator < 3;
}

void PortfolioOnlineGenome::calculateNextMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec)
{
	if(player == _player && !hasMoreMoves())
	{
		System::FatalError("Trying tom compute more moves than there is available in current genome.");
	}

	if(player == _player)
	{
		calculateMoves(player, moves, state, moveVec, _iterator);
		_iterator++;
	}
	else
	{
		calculateMoves(player, moves, state, moveVec, 0);
	}
}

void PortfolioOnlineGenome::calculateMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec)
{
	calculateMoves(player, moves, state, moveVec, 0);
	/*
	// generate all script moves for this player at this state and store them in allScriptMoves
	for (size_t scriptIndex(0); scriptIndex<_scriptVec[player].size(); ++scriptIndex)
	{
		// get the associated player pointer
		const PlayerPtr & pp = getPlayerPtr(player, scriptIndex);

		// get the actual script we are working with
		const IDType actualScript = getScript(player, scriptIndex);

		// generate the moves inside the appropriate vector
		getMoves(player, actualScript).clear();
		pp->getMoves(state, moves, getMoves(player, actualScript));
	}

	// for each unit the player has to move, populate the move vector with the appropriate script move
	for (size_t unitIndex(0); unitIndex < moves.numUnits(); ++unitIndex)
	{
		// the unit from the state
		const Unit & unit = state.getUnit(player, unitIndex);

		// the move it would choose to do based on its associated script preference
		Action unitMove = getMove(player, unitIndex, getUnitScript(unit));

		// put the unit into the move vector
		moveVec.push_back(unitMove);
	}
	 */
}

const IDType & PortfolioOnlineGenome::getUnitScript(const IDType & player, const int & id) const
{
	//return (*_unitScriptMap[player][0].find(id)).second;
	return getUnitScript(player, id, 0);
}

const IDType & PortfolioOnlineGenome::getUnitScript(const Unit & unit) const
{
	return getUnitScript(unit.player(), unit.ID(), 0);
}

const IDType & PortfolioOnlineGenome::getUnitScript(const IDType & player, const int & id, const int level) const
{
	return (*_unitScriptMap[player][level].find(id)).second;
}

const IDType & PortfolioOnlineGenome::getUnitScript(const Unit & unit, const int level) const
{
	return getUnitScript(unit.player(), unit.ID(), level);
}

const IDType & PortfolioOnlineGenome::getScript(const IDType & player, const size_t & index)
{
	return _scriptVec[player][index];
}

const PlayerPtr & PortfolioOnlineGenome::getPlayerPtr(const IDType & player, const size_t & index)
{
	return _playerPtrVec[player][index];
}

const size_t PortfolioOnlineGenome::getNumScripts(const IDType & player) const
{
	return _scriptSet[player].size();
}

void PortfolioOnlineGenome::setUnitScript(const IDType & player, const int & id, const IDType & script, const int level)
{
	if (_scriptSet[player].find(script) == _scriptSet[player].end())
	{
		_scriptSet[player].insert(script);
		_scriptVec[player].push_back(script);
		_playerPtrVec[player].push_back(PlayerPtr(AllPlayers::getPlayerPtr(player, script)));
	}
	_unitScriptMap[player][level][id] = script;
}

void PortfolioOnlineGenome::setUnitScript(const Unit & unit, const IDType & script, const int level)
{
	setUnitScript(unit.player(), unit.ID(), script, level);
}

void PortfolioOnlineGenome::setUnitScript(const IDType & player, const int & id, const IDType & script)
{
	setUnitScript(player, id, script, 0);
	/*
	if (_scriptSet[player].find(script) == _scriptSet[player].end())
	{
		_scriptSet[player].insert(script);
		_scriptVec[player].push_back(script);
		_playerPtrVec[player].push_back(PlayerPtr(AllPlayers::getPlayerPtr(player, script)));
	}
	_unitScriptMap[player][0][id] = script;
	 */
}

void PortfolioOnlineGenome::setUnitScript(const Unit & unit, const IDType & script)
{
	setUnitScript(unit.player(), unit.ID(), script, 0);
}

void PortfolioOnlineGenome::setFitness(const StateEvalScore & fitness)
{
	this->_fitness = fitness;
}

const StateEvalScore & PortfolioOnlineGenome::getFitness() const
{
	return this->_fitness;
}

void PortfolioOnlineGenome::printScripts(const IDType & player, const GameState & state)
{
	IDType players[2] = {Players::Player_One, Players::Player_Two};
	for(int p = 0; p < 2; p++)
	{
		std::cout << "Player: " << (int) players[p] << std::endl;
		for(int level = 0; level < 3; level++)
		{
			std::cout << "Level:" << level + 1;
			for (size_t unitIndex(0); unitIndex < state.numUnits(players[p]); ++unitIndex)
			{
				IDType script(getUnitScript(state.getUnit(players[p], unitIndex), level));
				std::cout << " " << (int)script;
			}
			std::cout << std::endl;
		}
	}
}
