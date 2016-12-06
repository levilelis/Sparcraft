#pragma once

#include "Common.h"
#include "Player.h"

#include "TranspositionTable.h"
#include "AlphaBetaSearch.h"
#include "AlphaBetaSearchParameters.hpp"
#include "AlphaBetaSearchResults.hpp"
#include "BeamLearner.h"

namespace SparCraft
{

class AlphaBetaSearch;
/*----------------------------------------------------------------------
 | Alpha Beta Player
 |----------------------------------------------------------------------
 | Runs Alpha Beta search given a set of search parameters
 `----------------------------------------------------------------------*/
class Player_AdaptiveBeamAlphaBeta : public Player
{
	AlphaBetaSearch * alphaBeta;
	TTPtr TT;
	AlphaBetaSearchParameters _params;
	BeamLearner * learner;

public:
	Player_AdaptiveBeamAlphaBeta (const IDType & playerID);
	Player_AdaptiveBeamAlphaBeta (const IDType & playerID, const AlphaBetaSearchParameters & params, TTPtr table);
    virtual ~Player_AdaptiveBeamAlphaBeta();
	void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
	void updateStat(GameState & state);
	void init(GameState & state, const int max_player);
	void choosePolicy(GameState & state);
	void finish();
	void setParameters(AlphaBetaSearchParameters & p);
	AlphaBetaSearchParameters & getParams();
	void setTranspositionTable(TTPtr table);
	AlphaBetaSearchResults & results();
	IDType getType() { return PlayerModels::AlphaBeta; }
};

}
