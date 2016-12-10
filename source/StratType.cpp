#include "StratType.h"

using namespace SparCraft;

StratType::StratType(const Unit & unit)
{
	this->_unitType = unit.type();
	this->hpLevel = (short)unit.currentHP() / ((short)unit.maxHP()/3);

//	std::cout << "max: " << (short)unit.maxHP() << " current hp: " << (short)unit.currentHP() << "hp level: " << this->hpLevel << std::endl;
}

bool StratType::operator==(const StratType & t) const
{
	return (this->_unitType == t._unitType && this->hpLevel == t._unitType);
}


