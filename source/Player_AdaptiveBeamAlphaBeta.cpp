#include "Player_AdaptiveBeamAlphaBeta.h"
#include "BeamLearner_EpsilonGreedy.h"
#include "BeamLearner_QLearning.h"

using namespace SparCraft;

Player_AdaptiveBeamAlphaBeta::Player_AdaptiveBeamAlphaBeta (const IDType & playerID)
{
	_playerID = playerID;
	learner = new BeamLearner_EpsilonGreedy(_params.isLearning(), _params.getBeamSize(), _params.getFilename());
}

Player_AdaptiveBeamAlphaBeta::Player_AdaptiveBeamAlphaBeta (const IDType & playerID, const AlphaBetaSearchParameters & params, TTPtr table)
{
	_playerID = playerID;
	_params = params;
	TT = table;

    alphaBeta = new AlphaBetaSearch(_params, TT);
	learner = new BeamLearner_EpsilonGreedy(_params.isLearning(), _params.getBeamSize(), _params.getFilename());
    //learner = new BeamLearner_QLearning();
}

Player_AdaptiveBeamAlphaBeta::~Player_AdaptiveBeamAlphaBeta()
{
	learner->writePolicyFile();

    delete alphaBeta;
    delete learner;
}

AlphaBetaSearchResults & Player_AdaptiveBeamAlphaBeta::results()
{
	return alphaBeta->getResults();
}

AlphaBetaSearchParameters & Player_AdaptiveBeamAlphaBeta::getParams()
{
	return _params;
}

void Player_AdaptiveBeamAlphaBeta::setParameters(AlphaBetaSearchParameters & p)
{
	_params = p;
}

void Player_AdaptiveBeamAlphaBeta::setTranspositionTable(TTPtr table)
{
	TT = table;
}

void Player_AdaptiveBeamAlphaBeta::choosePolicy(GameState & state)
{
	_params.setMaxChildren(learner->getBeamSize(state, _params.maxPlayer()));
//	_params.setMaxChildren(20);
}

void Player_AdaptiveBeamAlphaBeta::updateStat(GameState & state)
{
	learner->update(state, _params.maxPlayer());
}

void Player_AdaptiveBeamAlphaBeta::init(GameState & state, const int max_player)
{
	learner->init(state, max_player);
}

void Player_AdaptiveBeamAlphaBeta::finish()
{
	learner->reset();
}

void Player_AdaptiveBeamAlphaBeta::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
    moveVec.clear();
	alphaBeta->doSearch(state);
    moveVec.assign(alphaBeta->getResults().bestMoves.begin(), alphaBeta->getResults().bestMoves.end());
}
