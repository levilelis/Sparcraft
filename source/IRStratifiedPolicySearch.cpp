#include "IRStratifiedPolicySearch.h"

using namespace SparCraft;

IRStratifiedPolicySearch::IRStratifiedPolicySearch(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit)
	: _player(player)
	, _enemyScript(enemyScript)
	, _iterations(iter)
    , _responses(responses)
    , _totalEvals(0)
    , _timeLimit(timeLimit)
{
	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);
	_playerScriptPortfolio.push_back(PlayerModels::Cluster);
//	_playerScriptPortfolio.push_back(PlayerModels::Kiter_NOKDPS);
//	_playerScriptPortfolio.push_back(PlayerModels::MoveBackward);
}

std::vector<Action> IRStratifiedPolicySearch::search(const IDType & player, const GameState & state)
{
	Timer t;
    t.start();
    _totalEvals = 0;

    const IDType enemyPlayer(state.getEnemy(player));

    // calculate the seed scripts for each player
    // they will be used to seed the initial root search
  //  IDType seedScript = calculateInitialSeed(player, state);
  //  IDType enemySeedScript = calculateInitialSeed(enemyPlayer, state);
    double ms = t.getElapsedTimeInMilliSec();
    int randomMax = 1;
    StateEvalScore bestScore;

    UnitScriptData originalScriptData;
    //setAllScripts(player, state, originalScriptData, seedScript);
    //setAllScripts(enemyPlayer, state, originalScriptData, enemySeedScript);
    setAllScripts(player, state, originalScriptData, PlayerModels::NOKDPS);
    setAllScripts(enemyPlayer, state, originalScriptData, PlayerModels::NOKDPS);

    UnitScriptData bestScriptData(originalScriptData);

    while(ms < _timeLimit)
    {
    	GameState copyState(state);
        UnitScriptData currentScriptData(originalScriptData);

     //   ms = t.getElapsedTimeInMilliSec();
     //   std::cout << "Before doing the search: " << ms << std::endl;
        StateEvalScore score = doStratifiedSearch(player, copyState, currentScriptData, randomMax, t);
    	//std::cout << "Score: " << score.val() << "\t randomMax: " << randomMax << std::endl;

		if(randomMax == 1 || score > bestScore)
		{
			bestScore = score;
			//bestScriptData = currentScriptData;
          //  for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
           // {
            //    const Unit & unit(state.getUnit(player, unitIndex));
             //   bestScriptData.setUnitScript(unit, currentScriptData.getUnitScript(unit));
                bestScriptData = currentScriptData;
           // }
         //   if(randomMax > 1)
          //  	std::cout << "Best scripts chosen for randomMax: " << randomMax << " with eval: " << score.val() << std::endl;
		}

		randomMax += 1;
		ms = t.getElapsedTimeInMilliSec();
	//	std::cout << "randomMax: " << randomMax << "\t eval: " << score.val() << "\t time: " << ms << std::endl;
    }
  //  std::cout << std::endl;

   // ms = t.getElapsedTimeInMilliSec();
  //  printf("\nMove IRSPS chosen in %lf ms\n", ms);

    // convert the script vector into a move vector and return it
	MoveArray moves;
	state.generateMoves(moves, player);
    std::vector<Action> moveVec;
    GameState copy(state);
    bestScriptData.calculateMoves(player, moves, copy, moveVec);

    return moveVec;
}


StateEvalScore IRStratifiedPolicySearch::doStratifiedSearch(const IDType & player, const GameState & state, UnitScriptData & currentScriptData, const int & randomMax, Timer & timer)
{
   // Timer t;
   // t.start();
	StateEvalScore finalScore;
    // the enemy of this player
    const IDType enemyPlayer(state.getEnemy(player));

    //compute the set of type for each unit that can move
    std::set<StratType> types;
    //std::map<Unit, StratType> unitTypes;
    std::map<StratType, std::vector<Unit> > typeUnits;
    for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
    {
        const Unit & unit(state.getUnit(player, unitIndex));
        StratType t(unit, state, randomMax);
        if(typeUnits.find(t) == typeUnits.end())
        {
        	std::vector<Unit> v;
        	typeUnits[t] = v;
        }
        typeUnits[t].push_back(unit);
        types.insert(t);
        //unitTypes[unit] = t;
    }

    for (size_t i(0); i<_iterations; ++i)
    //while(timer.getElapsedTimeInMilliSec() < _timeLimit)
    {
        // set up data for best scripts
        IDType          bestScriptVec[types.size()];
	    StateEvalScore  bestScoreVec[types.size()];

	    std::set<StratType>::iterator it = types.begin();
        for(int typeIndex = 0; typeIndex < types.size(); typeIndex++, ++it)
        {
            for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
            {
            	for(int j = 0; j < typeUnits[*it].size(); j++)
            	{
                    currentScriptData.setUnitScript(typeUnits[*it][j], _playerScriptPortfolio[sIndex]);
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

                // if we have a better score, set it
                if (sIndex == 0 || score > bestScoreVec[typeIndex])
                {
                    bestScriptVec[typeIndex] = _playerScriptPortfolio[sIndex];
                    bestScoreVec[typeIndex]  = score;
                    finalScore = score;
                }
            }

        	for(int j = 0; j < typeUnits[*it].size(); j++)
        	{
                currentScriptData.setUnitScript(typeUnits[*it][j], bestScriptVec[typeIndex]);
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

            if (_timeLimit > 0 && timer.getElapsedTimeInMilliSec() > _timeLimit)
            {
            	return finalScore;
            }
        }

        //std::cout << "Finished on iteration SPS" << std::endl;
    }

    return finalScore;
}

/*
StateEvalScore IRStratifiedPolicySearch::doStratifiedSearch(const IDType & player, const GameState & state, UnitScriptData & currentScriptData, const int & randomMax, Timer & timer)
{
 //   Timer t;
 //   t.start();

//    StateEvalScore finalScore = eval(player, state, currentScriptData);
	StateEvalScore finalScore;
    // the enemy of this player
    const IDType enemyPlayer(state.getEnemy(player));
    for (size_t i(0); i<_iterations; ++i)
    {
        //compute the set of type for each unit that can move
        std::set<StratType> types;
        std::map<StratType, std::vector<Unit> > typeUnits;
        for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
        {
            const Unit & unit(state.getUnit(player, unitIndex));
            StratType t(unit, state);
            if(typeUnits.find(t) == typeUnits.end())
            {
            	std::vector<Unit> v;
            	typeUnits[t] = v;
            }
            typeUnits[t].push_back(unit);
            types.insert(t);
            //unitTypes[unit] = t;
        }

      //  std::cout << "Number of types: " << types.size() << " with maximum random: " << randomMax << std::endl;

        // set up data for best scripts
        IDType          bestScriptVec[types.size()];
	    StateEvalScore  bestScoreVec[types.size()];

	    std::set<StratType>::iterator it = types.begin();
        for(int typeIndex = 0; typeIndex < types.size(); typeIndex++, ++it)
        {
            for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
            {
            	for(int j = 0; j < typeUnits[*it].size(); j++)
            	{
                    currentScriptData.setUnitScript(typeUnits[*it][j], _playerScriptPortfolio[sIndex]);
            	}

                StateEvalScore score = eval(player, state, currentScriptData);

                // if we have a better score, set it
                if (sIndex == 0 || score > bestScoreVec[typeIndex])
                {
                    bestScriptVec[typeIndex] = _playerScriptPortfolio[sIndex];
                    bestScoreVec[typeIndex]  = score;
                    finalScore = score;
                }
            }

        	for(int j = 0; j < typeUnits[*it].size(); j++)
        	{
                currentScriptData.setUnitScript(typeUnits[*it][j], bestScriptVec[typeIndex]);
        	}

            if (_timeLimit > 0 && timer.getElapsedTimeInMilliSec() > _timeLimit)
            {
            	return finalScore;
            }
        }
    }

    return finalScore;
}
*/

IDType IRStratifiedPolicySearch::calculateInitialSeed(const IDType & player, const GameState & state)
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

StateEvalScore IRStratifiedPolicySearch::eval(const IDType & player, const GameState & state, UnitScriptData & playerScriptsChosen)
{
    const IDType enemyPlayer(state.getEnemy(player));

	Game g(state, 100);

    //g.playIndividualScripts(playerScriptsChosen);

    _totalEvals++;
    return g.playLimitedIndividualScripts(player, playerScriptsChosen, 4);

	//return g.getState().eval(player, SparCraft::EvaluationMethods::LTD2);
}

void  IRStratifiedPolicySearch::setAllScripts(const IDType & player, const GameState & state, UnitScriptData & data, const IDType & script)
{
    for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
    {
        data.setUnitScript(state.getUnit(player, unitIndex), script);
    }
}
