#pragma once

#include <ScenarioClass.h>
#include <YRMathVector.h>

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
