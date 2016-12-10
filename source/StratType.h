#pragma once

#include "Common.h"
#include "PlayerProperties.h"
#include "UnitProperties.h"
#include "UnitType.h"
#include "Unit.h"
#include <iostream>

namespace SparCraft
{

class Unit;

class StratType
{
private:
	BWAPI::UnitType _unitType;
	int hpLevel;

public:
	StratType(const Unit & unit);
    bool operator==(const StratType & t) const;
	bool friend operator<(const StratType & t1, const StratType & t2)
	{
		if(t1._unitType != t2._unitType)
		{
			return t1._unitType < t2._unitType;
		}

		return t1.hpLevel < t2.hpLevel;
	}
};

}
