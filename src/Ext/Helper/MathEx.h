#pragma once

#include <string>
#include <vector>
#include <map>

#include <ScenarioClass.h>
#include <YRMathVector.h>

static Randomizer GetRandom()
{
	return ScenarioClass::Instance->Random;
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
		return GetRandom().RandomRanged(min, max);
	}
	return defVal;
}

static std::map<Point2D, int> MakeTargetPad(std::vector<int> weights, int count, int& maxValue)
{
	int weightCount = weights.size();
	std::map<Point2D, int> targetPad{};
	maxValue = 0;

	// 将所有的概率加起来，获得上游指标
	for (int index = 0; index < count; index++)
	{
		Point2D target{};
		target.X = maxValue;
		int weight = 1;
		if (weightCount > 0 && index < weightCount)
		{
			int w = weights[index];
			if (w > 0)
			{
				weight = w;
			}
		}
		maxValue += weight;
		target.Y = maxValue;
		targetPad[target] = index;
	}
	return targetPad;
}

static int Hit(std::map<Point2D, int> targetPad, int maxValue)
{
	int index = 0;
	int p = GetRandom().RandomRanged(0, maxValue);
	for (auto it : targetPad)
	{
		Point2D tKey = it.first;
		if (p >= tKey.X && p < tKey.Y)
		{
			index = it.second;
			break;
		}
	}
	return index;
}

static bool Bingo(double chance)
{
	if (chance > 0)
	{
		return chance >= 1 || chance >= GetRandom().RandomDouble();
	}
	return false;
}

static bool Bingo(std::vector<double> chances, int index)
{
	int size = chances.size();
	if (size < index + 1)
	{
		return true;
	}
	double chance = chances[index];
	return Bingo(chance);
}
