#include "StratType.h"
#include "Unit.h"

using namespace SparCraft;

StratType::StratType()
{
	this->_unitType = -1;
	this->_hpLevel = -1;
	this->_random = -1;
}

StratType::StratType(const Unit & unit, const GameState & state)
{
	this->_unitType = unit.type();
	this->_hpLevel = (short)unit.currentHP() / ((short)unit.maxHP()/3);
	this->_random = -1;
}

StratType::StratType(const Unit & unit, const GameState & state, int maxRandom)
{
	this->_unitType = unit.type();
	this->_hpLevel = (short)unit.currentHP() / ((short)unit.maxHP()/3);

	if(maxRandom > 0)
	{
		this->_random = rand() % maxRandom;
	}
	else
	{
		this->_random = -1;
	}
}

bool StratType::operator==(const StratType & t) const
{
	return (this->_unitType == t._unitType
			&& this->_hpLevel == t._hpLevel
			&& this->_random == t._random);
}

void StratType::print() const
{
	std::cout << this->_unitType.getName() << "\t" << this->_hpLevel << std::endl;
}


