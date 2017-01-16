#pragma once

#include "Common.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "UnitType.h"
//#include "Unit.h"
#include "GameState.h"
#include <iostream>

namespace SparCraft
{

class Unit;
//class GameState;

class StratType
{
private:
	BWAPI::UnitType _unitType;
	int _hpLevel;
	int _random;

public:

	StratType(const Unit & unit, const GameState & state);
	StratType(const Unit & unit, const GameState & state, int maxRandom);
	StratType();
    bool operator==(const StratType & t) const;
    void print() const;
	bool friend operator<(const StratType & t1, const StratType & t2)
	{
		if(t1._unitType != t2._unitType)
		{
			return t1._unitType < t2._unitType;
		}
		if(t1._random != t2._random)
		{
			return t1._random < t2._random;
		}
		return t1._hpLevel < t2._hpLevel;
	}
};

}
