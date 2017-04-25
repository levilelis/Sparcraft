#include "StratType.h"
#include "Unit.h"

using namespace SparCraft;

StratType::StratType()
{
	this->_unitType = -1;
	this->_hpLevel = -1;
	this->_random = -1;
}
/*
StratType::StratType(const Unit & unit, const GameState & state)
{
	this->_unitType = unit.type();
	this->_hpLevel = (short)unit.currentHP() / ((short)unit.maxHP()/3);
	this->_random = -1;
}
*/

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

StratType::StratType(const Unit & unit, const GameState & state, int maxRandom, int hpLevelDiv)
{
	//single type
	if(hpLevelDiv == -1)
	{
		this->_unitType = -1;
		this->_hpLevel = -1;
		this->_random = -1;
	}
	else if(hpLevelDiv == 0)
	{
		//if the unit is a Terran Marine or a Protoss Dragoon set it as ranged
		if(unit.type().getID() == 0 || unit.type().getID() == 66)
		{
			this->_unitType = 0;
		}
		else
		{
			this->_unitType = 1;
		}
		this->_hpLevel = -1;
		this->_random = -1;
	}
	else
	{
		this->_unitType = unit.type();
		this->_hpLevel = (short)unit.currentHP() / ((short)unit.maxHP()/hpLevelDiv);

		if(maxRandom > 0)
		{
			this->_random = rand() % maxRandom;
		}
		else
		{
			this->_random = -1;
		}
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


