#include "AdaptableStratType.h"
#include "Unit.h"

using namespace SparCraft;

int AdaptableStratType::_knob = 3;
bool AdaptableStratType::_singleType = false;
bool AdaptableStratType::_attackType = false;
int AdaptableStratType::_counter = 0;
std::vector<int> AdaptableStratType::_numberTypes;

AdaptableStratType::AdaptableStratType()
{
	this->_unitType = -1;
	this->_hpLevel = -1;
}

void AdaptableStratType::printType()
{
	if(_singleType)
	{
		std::cout << -1 << std::endl;
	}
	else if(_attackType)
	{
		std::cout << 0 << std::endl;
	}
	else
	{
		std::cout << _knob + 1 << std::endl;
	}
}

AdaptableStratType::AdaptableStratType(const Unit & unit, const GameState & state)
{
	if(_singleType)
	{
		this->_unitType = -1;
		this->_hpLevel = -1;
	}
	else if(_attackType)
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
	}
	else
	{
		//std::cout << _knob << std::endl;

		this->_unitType = unit.type();
		if(_knob > 0)
		{
			this->_hpLevel = (short)unit.currentHP() / ((short)unit.maxHP()/_knob);
		}
		else
		{
			this->_hpLevel = -1;
		}
	}
}

bool AdaptableStratType::operator==(const AdaptableStratType & t) const
{
	return (this->_unitType == t._unitType
			&& this->_hpLevel == t._hpLevel);
}

void AdaptableStratType::print() const
{
	std::cout << this->_unitType.getName() << "\t" << this->_hpLevel << std::endl;
}

void AdaptableStratType::increase(float timePlayout, int timeLimit, int portfolioSize)
{
	//if the number of types is empty then the type system is at its finest form
	if(_numberTypes.empty())
	{
		return;
	}

	//estimating whether the search algorithm will be able to handle a finer type system
	if(*(_numberTypes.rbegin()) * timePlayout * portfolioSize < timeLimit)
	{
		_numberTypes.erase(_numberTypes.end() - 1, _numberTypes.end());
		if(_singleType)
		{
			_singleType = false;
		}
		else if(_attackType)
		{
			_attackType = false;
		}
		else if(_knob < 3)
		{
			_knob++;
		}
	}
}

void AdaptableStratType::decrease(int numberTypes)
{
	//if hasn't reached the coarsest type system
	if(!_singleType)
	{
		_numberTypes.push_back(numberTypes);
	}

	if(_knob > 0)
	{
		_knob--;
	}
	else if(!_attackType)
	{
		_attackType = true;
	}
	else
	{
		_singleType = true;
	}
}
