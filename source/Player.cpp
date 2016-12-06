#include "Player.h"

using namespace SparCraft;


void Player::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
	// not implemented
}

void Player::updateStat(GameState & state)
{
	// not implemented
}

void Player::init(GameState & state, const int max_player)
{
	// not implemented
}

void Player::choosePolicy(GameState & state)
{
	//not implemented
}

void Player::finish()
{
	// not implemented
}


const IDType Player::ID() 
{ 
	return _playerID; 
}

void Player::setID(const IDType & playerID)
{
	_playerID = playerID;
}
