#pragma once

#include <string>
#include <algorithm>

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

//
//  Lowercases string
//
template <typename T>
static std::basic_string<T> lowercase(const std::basic_string<T>& s)
{
    std::basic_string<T> s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(),
        [](const T v){ return static_cast<T>(std::tolower(v)); });
    return s2;
}

//
// Uppercases string
//
template <typename T>
static std::basic_string<T> uppercase(const std::basic_string<T>& s)
{
    std::basic_string<T> s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(),
        [](const T v){ return static_cast<T>(std::toupper(v)); });
    return s2;
}

static std::string& trim(std::string& s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

static void split(std::string& s, std::string& delim, std::vector<std::string>* result)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		result->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		result->push_back(s.substr(last, index - last));
	}
}

static bool IsNotNone(std::string val)
{
	return !val.empty() && lowercase(trim(val)) != "none";
}

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
