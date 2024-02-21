#pragma once

#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <random>

#include <ScenarioClass.h>
#include <YRMathVector.h>

/**
 *@brief ScenarioClass::Instance->Random Only get the Same number in One frame
 *
 */
class Random
{
public:
	static void SetRandomSeed(int seed)
	{
		_engine.seed(seed);
	}

	/**
	 *@brief Include maximum value
	 *
	 * @param min
	 * @param max
	 * @return int
	 */
	static int RandomRanged(int min, int max)
	{
		std::uniform_int_distribution<int> dis(min, max);
		return dis(_engine);
	}

	static double RandomDouble()
	{
		return RandomRanged(1, INT_MAX) / (double)((unsigned int)INT_MAX + 1);
	}
private:
	inline static std::minstd_rand _engine{};
};

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
		return Random::RandomRanged(min, max);
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
	int p = Random::RandomRanged(0, maxValue);
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
		return chance >= 1 || chance >= Random::RandomDouble();
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

static bool CheckOnMarks(std::vector<std::string> values, std::vector<std::string> marks)
{
	// 取交集
	std::set<std::string> m(marks.begin(), marks.end());
	std::set<std::string> t(values.begin(), values.end());
	std::set<std::string> v;
	std::set_intersection(m.begin(), m.end(), t.begin(), t.end(), std::inserter(v, v.begin()));
	return !v.empty();
}
