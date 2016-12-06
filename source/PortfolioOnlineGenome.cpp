#include "PortfolioOnlineGenome.h"

using namespace SparCraft;

PortfolioOnlineGenome::PortfolioOnlineGenome(const GameState & state)
        : _mutateRate(0.2),
          _rand(0, std::numeric_limits<int>::max(), Constants::Seed_Player_Random_Time ? static_cast<unsigned int>(std::time(0)) : 0),
          _fitness(StateEvalScore())
{
	IDType players[2] = {Players::Player_One, Players::Player_Two};
        //initializing the script of all units for both players with NO-OVERKILL
        for(int p = 0; p < 2; p++)
        {
                for (size_t unitIndex(0); unitIndex < state.numUnits(players[p]); ++unitIndex)
                {
                        setUnitScript(state.getUnit(players[p], unitIndex), PlayerModels::NOKDPS);
                }
        }
}

void PortfolioOnlineGenome::mutate(const IDType & player, const GameState & state, std::vector<IDType> & portfolio)
{
        for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
        {
                if(_rand.nextInt() % 100 < (this->_mutateRate * 100))
                {
                        int replace = _rand.nextInt() % portfolio.size();
                	setUnitScript(state.getUnit(player, unitIndex), portfolio[replace]);
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

void PortfolioOnlineGenome::calculateMoves(const IDType & player, MoveArray & moves, GameState & state, std::vector<Action> & moveVec)
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
        Action unitMove = getMove(player, unitIndex, getUnitScript(unit));

        // put the unit into the move vector
        moveVec.push_back(unitMove);
    }
}

const IDType & PortfolioOnlineGenome::getUnitScript(const IDType & player, const int & id) const
{
    return (*_unitScriptMap[player].find(id)).second;
}
    
const IDType & PortfolioOnlineGenome::getUnitScript(const Unit & unit) const
{
    return getUnitScript(unit.player(), unit.ID());
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

void PortfolioOnlineGenome::setUnitScript(const IDType & player, const int & id, const IDType & script)
{
    if (_scriptSet[player].find(script) == _scriptSet[player].end())
    {
        _scriptSet[player].insert(script);
        _scriptVec[player].push_back(script);
        _playerPtrVec[player].push_back(PlayerPtr(AllPlayers::getPlayerPtr(player, script)));
    }
        
    _unitScriptMap[player][id] = script;
}

void PortfolioOnlineGenome::setUnitScript(const Unit & unit, const IDType & script)
{
    setUnitScript(unit.player(), unit.ID(), script);
}

void PortfolioOnlineGenome::setFitness(const StateEvalScore & fitness)
{
        this->_fitness = fitness;
}

const StateEvalScore & PortfolioOnlineGenome::getFitness() const
{
        return this->_fitness;
}
