#include "PaintballState.h"

#include <Ext/Helper/MathEx.h>

void PaintballState::RGBIsPower()
{
	if (!_rgbMode)
	{
		_rgbMode = true;
	}
	if (!IsActive())
	{
		PaintballData rgb;
		rgb.SetColor(Colors::Red);
		Start(&rgb);
	}
	if (_rgbTimer.Expired())
	{
		switch (_rgbIdx)
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
		_rgbIdx++;
		if (_rgbIdx > 2)
		{
			_rgbIdx = 0;
		}
		_rgbTimer.Start(15);
	}
	// 不灵不灵
	Data.SetBrightMultiplier(static_cast<float>(Random::RandomRanged(5, 15) / 10));
	Reset();
}

bool PaintballState::NeedPaint(bool& changeColor, bool& changeBright)
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

void PaintballState::OnInitState(bool replace)
{
	PaintballData* data = GetInitData();
	if (data->Enable)
	{
		StateScript<PaintballData>::OnInitState(replace);
	}
	else if (_rgbMode)
	{
		RGBIsPower();
	}
}

void PaintballState::OnUpdate()
{
#ifdef DEBUG
	StateScript<PaintballData>::OnUpdate();
#endif // DEBUG
	if (_rgbMode)
	{
		RGBIsPower();
	}
}

