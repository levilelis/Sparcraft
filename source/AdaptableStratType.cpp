#include "AdaptableStratType.h"
#include "Unit.h"

using namespace SparCraft;

int AdaptableStratType::_knob = 3;
bool AdaptableStratType::_singleType = false;
int AdaptableStratType::_counter = 0;
std::vector<int> AdaptableStratType::_numberTypes;

AdaptableStratType::AdaptableStratType()
{
	this->_unitType = -1;
	this->_hpLevel = -1;
}

AdaptableStratType::AdaptableStratType(const Unit & unit, const GameState & state)
{
	if(_singleType)
	{
		this->_unitType = -1;
		this->_hpLevel = -1;
	}
	else
	{
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
	//if(_counter > 0)
	//{
	//	_counter--;
	//	return;
	//}
//	std::cout << "Knob: " << _knob << " Single? "<< _singleType << std::endl;

	//if the number of types is empty then the type system is at its finest form
	if(_numberTypes.empty())
	{
		return;
	}

	//std::cout << "INCREASE" << std::endl;
	//if(!_numberTypes.empty())
	//	std::cout << "Types: " << *(_numberTypes.rbegin()) << " Time: " << timePlayout << std::endl;

	if(*(_numberTypes.rbegin()) * timePlayout * portfolioSize < timeLimit)
	{
//		std::cout << "INCREASING" << std::endl;
		_numberTypes.erase(_numberTypes.end() - 1, _numberTypes.end());
		if(_singleType)
		{
			_singleType = false;
		}
		else
		{
			if(_knob < 3)
			{
				_knob++;
			}
		}
	}
//	else
//	{
//		std::cout << "Can't increase, currently knob: " << _knob << std::endl;
//	}
}
void AdaptableStratType::decrease(int numberTypes)
{
//	std::cout << "Knob: " << _knob << " Single? "<< _singleType << std::endl;
	//if hasn't reached the coarsest type system
	if(!_singleType)
	{
//		std::cout << "DECREASING" << std::endl;
		_numberTypes.push_back(numberTypes);
	}

	if(_knob > 0)
	{
		_knob--;
	}
	else
	{
		_singleType = true;
	}

	//_counter = 20;
}
