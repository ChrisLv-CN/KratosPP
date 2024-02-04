#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Helper/MathEx.h>

class ProximityRangeData : public INIConfig
{
public:
	// 提前引爆
	int Range = 0; // *256
	bool Random = false;
	Point2D RandomRange = Point2D::Empty;

	virtual void Read(INIBufferReader* reader) override
	{
		double range = reader->Get<double>("ProximityRange", -1);
		if (range > 0)
		{
			Range = (int)(range * 256);
		}
		Vector2D<double> randomRange = reader->Get<Vector2D<double>>("ProximityRange.Random", Vector2D<double>::Empty);
		if (0 != randomRange.X && 0 != randomRange.Y)
		{
			int x = (int)(randomRange.X * 256);
			int y = (int)(randomRange.Y * 256);
			if (x < 0)
			{
				x = -x;
			}
			if (y < 0)
			{
				y = -x;
			}
			if (x > y)
			{
				int t = x;
				x = y;
				y = t;
			}
			RandomRange = { x, y };
			Random = 0 != RandomRange.X && 0 != RandomRange.Y;
		}
	}

	int GetRange()
	{
		if (Random && !RandomRange.IsEmpty())
		{
			return GetRandomValue(RandomRange, 0);
		}
		return Range;
	}
};
