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

class AdaptableStratType
{
private:
	BWAPI::UnitType _unitType;
	int _hpLevel;
	static int _knob;
	static bool _singleType;
	static bool _attackType;
	static int _counter;
	static std::vector<int> _numberTypes;

public:

	AdaptableStratType(const Unit & unit, const GameState & state);
	AdaptableStratType();
    bool operator==(const AdaptableStratType & t) const;
    void print() const;
    static void increase(float timePlayout, int timeLimit, int portfolioSize);
    static void decrease(int numberTypes);
    static void printType();
	bool friend operator<(const AdaptableStratType & t1, const AdaptableStratType & t2)
	{
		if(t1._unitType != t2._unitType)
		{
			return t1._unitType < t2._unitType;
		}
		return t1._hpLevel < t2._hpLevel;
	}
};

}
