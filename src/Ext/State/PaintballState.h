#pragma once

#include "State.h"
#include "PaintballData.h"

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
		if (_rgbMode)
		{
			RGBIsPower();
		}
	}

	void RGBIsPower()
	{
		if (!_rgbMode)
		{
			_rgbMode = true;
		}
		if (!IsActive())
		{
			PaintballData rgb;
			rgb.SetColor(Colors::Red);
			Enable(rgb);
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

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->_rgbMode)
			.Process(this->_rgbIdx)
			.Process(this->_rgbTimer)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		State<PaintballData>::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		State<PaintballData>::Save(stream);
		return const_cast<PaintballState*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	bool _rgbMode = false;
	int _rgbIdx = 0;
	CDTimerClass _rgbTimer;
};
