#include "StratifiedPolicySearch.h"

using namespace SparCraft;

StratifiedPolicySearch::StratifiedPolicySearch(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit, const size_t & hpLevelDiv)
	: _player(player)
	, _enemyScript(enemyScript)
	, _iterations(iter)
    , _responses(responses)
    , _totalEvals(0)
    , _timeLimit(timeLimit)
	, _hpLevelDiv(hpLevelDiv)
{
	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);
	_playerScriptPortfolio.push_back(PlayerModels::Cluster);
//	_playerScriptPortfolio.push_back(PlayerModels::Kiter_NOKDPS);
//	_playerScriptPortfolio.push_back(PlayerModels::MoveBackward);
}

std::vector<Action> StratifiedPolicySearch::search(const IDType & player, const GameState & state)
{
    Timer t;
    t.start();

    const IDType enemyPlayer(state.getEnemy(player));

    // calculate the seed scripts for each player
    // they will be used to seed the initial root search
//    IDType seedScript = calculateInitialSeed(player, state);
//    IDType enemySeedScript = calculateInitialSeed(enemyPlayer, state);

    // set up the root script data
    UnitScriptData originalScriptData;
//    setAllScripts(player, state, originalScriptData, seedScript);
//    setAllScripts(enemyPlayer, state, originalScriptData, enemySeedScript);
    setAllScripts(player, state, originalScriptData, PlayerModels::NOKDPS);
    setAllScripts(enemyPlayer, state, originalScriptData, PlayerModels::NOKDPS);

    double ms = t.getElapsedTimeInMilliSec();
//    printf("\nFirst Part %lf ms\n", ms);

    // do the initial root portfolio search for our player
    UnitScriptData currentScriptData(originalScriptData);

//    ms = t.getElapsedTimeInMilliSec();
//    std::cout << "Before doing the search: " << ms << std::endl;
    doStratifiedSearch(player, state, currentScriptData, t);
/*
    // iterate as many times as required
    for (size_t i(0); i<_responses; ++i)
    {
    	//printf("Response %lu \n", i+1);
        // do the portfolio search to improve the enemy's scripts
        doStratifiedSearch(enemyPlayer, state, currentScriptData);

        // then do portfolio search again for us to improve vs. enemy's update
        doStratifiedSearch(player, state, currentScriptData);
    }
*/
 //   ms = t.getElapsedTimeInMilliSec();

/*
    _fileTime.open("SPS.txt", std::ostream::app);
    if (!_fileTime.is_open())
    {
    	std::cout << "ERROR Opening file" << std::endl;
    }
    _fileTime << ms << ", ";
    _fileTime.close();
*/
//    printf("\nMove SPS chosen in %lf ms\n", ms);

    // convert the script vector into a move vector and return it
	MoveArray moves;
	state.generateMoves(moves, player);
    std::vector<Action> moveVec;
    GameState copy(state);
    currentScriptData.calculateMoves(player, moves, copy, moveVec);

    _totalEvals = 0;

    return moveVec;
}

void StratifiedPolicySearch::doStratifiedSearch(const IDType & player, const GameState & state, UnitScriptData & currentScriptData, Timer & timer)
{
   // Timer t;
   // t.start();

	int numberEvals = 0;

    // the enemy of this player
    const IDType enemyPlayer(state.getEnemy(player));

    //compute the set of type for each unit that can move
    //std::set<StratType> types;
    //std::map<Unit, StratType> unitTypes;
    std::map<StratType, std::vector<size_t> > typeUnits;
    for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
    {
        const Unit & unit(state.getUnit(player, unitIndex));
        //StratType t(unit, state, 0, _hpLevelDiv);
        //StratType t(unit, state, 0);
        StratType t(unit, state);
        if(typeUnits.find(t) == typeUnits.end())
        {
        	std::vector<size_t> v;
        	typeUnits[t] = v;
        }
        typeUnits[t].push_back(unitIndex);
      //  types.insert(t);
        //unitTypes[unit] = t;
    }
    
   // std::cout << "Number of types: " << typeUnits.size() << std::endl;

    //for (size_t i(0); i<_iterations; ++i)
    while(timer.getElapsedTimeInMilliSec() < _timeLimit)
    {
        // set up data for best scripts
       // IDType          bestScriptVec[types.size()];
	   // StateEvalScore  bestScoreVec[types.size()];
    	IDType          bestScriptVec[typeUnits.size()];
    	StateEvalScore  bestScoreVec[typeUnits.size()];

	    //std::set<StratType>::iterator it = types.begin();
    	std::map<StratType, std::vector<size_t> >::iterator it = typeUnits.begin();
        for(int typeIndex = 0; typeIndex < typeUnits.size(); typeIndex++, ++it)
        {
            for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
            {
            	for(int j = 0; j < (it->second).size()/*typeUnits[*it].size()*/; j++)
            	{
                    currentScriptData.setUnitScript(state.getUnit(player, (it->second)[j]), _playerScriptPortfolio[sIndex]);
            	}

            	/*
                for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
                {
                    const Unit & unit(state.getUnit(player, unitIndex));
                    //StratType t(unit, state);

                    if(unitTypes[unit] == *it)
                    {
                        currentScriptData.setUnitScript(unit, _playerScriptPortfolio[sIndex]);
                    }
                }*/

                StateEvalScore score = eval(player, state, currentScriptData);
                numberEvals++;

                // if we have a better score, set it
                if (sIndex == 0 || score > bestScoreVec[typeIndex])
                {
                    bestScriptVec[typeIndex] = _playerScriptPortfolio[sIndex];
                    bestScoreVec[typeIndex]  = score;
                }
            }

        	for(int j = 0; j < (it->second).size(); j++)
        	{
                currentScriptData.setUnitScript(state.getUnit(player, (it->second)[j]), bestScriptVec[typeIndex]);
        	}

            if (_timeLimit > 0 && timer.getElapsedTimeInMilliSec() > _timeLimit)
            {
             //	std::cout << "Number evals: " << numberEvals << std::endl;
            //	std::cout << state.numUnits(player) << ": " << timer.getElapsedTimeInMilliSec() << " ms." << std::endl;
            	return;
            }

            /*
            for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
            {
                const Unit & unit(state.getUnit(player, unitIndex));
               // StratType t(unit, state);

                if(unitTypes[unit] == *it)
                {
                    currentScriptData.setUnitScript(unit, bestScriptVec[typeIndex]);
                }
            }*/
        }
    }

	//std::cout << state.numUnits(player) << ": " << timer.getElapsedTimeInMilliSec() << " ms." << std::endl;
    //std::cout << "Number evals (COMPLETE): " << numberEvals << std::endl;

}

IDType StratifiedPolicySearch::calculateInitialSeed(const IDType & player, const GameState & state)
{
    IDType bestScript;
    StateEvalScore bestScriptScore;
    const IDType enemyPlayer(state.getEnemy(player));
    
    // try each script in the portfolio for each unit as an initial seed
    for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
    {
        UnitScriptData currentScriptData;
    
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
}

StateEvalScore StratifiedPolicySearch::eval(const IDType & player, const GameState & state, UnitScriptData & playerScriptsChosen)
{
    const IDType enemyPlayer(state.getEnemy(player));

	Game g(state, 100);

    //g.playIndividualScripts(playerScriptsChosen);

    _totalEvals++;
    return g.playLimitedIndividualScripts(player, playerScriptsChosen, 4);

	//return g.getState().eval(player, SparCraft::EvaluationMethods::LTD2);
}

void  StratifiedPolicySearch::setAllScripts(const IDType & player, const GameState & state, UnitScriptData & data, const IDType & script)
{
    for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
    {
        data.setUnitScript(state.getUnit(player, unitIndex), script);
    }
}
