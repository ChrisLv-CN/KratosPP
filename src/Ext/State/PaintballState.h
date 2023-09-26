#pragma once

#include "State.h"
#include "PaintballData.h"

#include <Common/EventSystems/EventSystem.h>

class PaintballState : public State<PaintballData>
{
public:
	virtual void StartTimer(int duration) override
	{
		State<PaintballData>::StartTimer(duration + 1);
	}

	bool NeedPaint(bool& changeColor, bool& changeBright)
	{
		changeColor = false;
		changeBright = false;
		if (IsActive())
		{
			changeColor = Data.ChangeColor;
			changeBright = Data.ChangeBright;
		}
		return changeColor || changeBright;
	}

	void Update()
	{
		if (rgbMode)
		{
			RGBIsPower();
		}
	}

	void RGBIsPower()
	{
		if (!rgbMode)
		{
			rgbMode = true;
		}
		if (!IsActive())
		{
			PaintballData rgb;
			rgb.SetColor(Colors::Red);
			Enable(rgb);
		}
		if (rgbTimer.Expired())
		{
			switch (rgbIdx)
			{
			case 0:
				Data.SetColor(Colors::Red);
				break;
			case 1:
				Data.SetColor(Colors::Green);
				break;
			case 2:
				Data.SetColor(Colors::Blue);
				break;
			}
			rgbIdx++;
			if (rgbIdx > 2)
			{
				rgbIdx = 0;
			}
			rgbTimer.Start(15);
		}
		// 不灵不灵
		Data.BrightMultiplier = static_cast<float>(GetRandom().RandomRanged(5, 15) / 10);
		Data.ChangeBright = Data.BrightMultiplier != 1.0f;
		Reset();
	}
private:
	bool rgbMode = false;
	int rgbIdx = 0;
	CDTimerClass rgbTimer;
};
