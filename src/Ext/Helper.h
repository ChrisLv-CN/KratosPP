#pragma once


#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/Component.h>
#include <Ext/Helper/Effect.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Status.h>
#include <Ext/Helper/Weapon.h>

#include <CellClass.h>
#include <MapClass.h>
#include <GeneralStructures.h>
#include <ScenarioClass.h>

template<typename T>
static int GetRandomValue(Vector2D<T> range, int defVal)
{
	int min = static_cast<int>(range.X);
	int max = static_cast<int>(range.Y);
	if (min > max)
	{
		int tmp = min;
		min = max;
		max = tmp;
	}
	if (max > 0)
	{
		return ScenarioClass::Instance->Random.RandomRanged(min, max);
	}
	return defVal;
}

static double DistanceFrom(CoordStruct sourcePos, CoordStruct targetPos, bool fullAirspace)
{
	if (fullAirspace)
	{
		CoordStruct tempSource = sourcePos;
		CoordStruct tempTarget = targetPos;
		tempSource.Z = 0;
		tempTarget.Z = 0;
		return tempSource.DistanceFrom(tempTarget);
	}
	return sourcePos.DistanceFrom(targetPos);
}

static bool InFog(CoordStruct location)
{
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		if (pCell->Flags & CellFlags::Revealed)
		{
			return false;
		}
	}
	return true;
}
