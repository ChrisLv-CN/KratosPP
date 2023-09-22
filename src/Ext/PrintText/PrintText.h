#pragma once

#include <string>
#include <map>

#include "PrintTextData.h"

#include <GeneralStructures.h>
#include <TacticalClass.h>

#include <Ext/Helper.h>

enum class LongText
{
	NONE = 0, HIT = 1, MISS = 2, CRIT = 3, GLANCING = 4, BLOCK = 5
};

static std::map<std::wstring, LongText> LongTextStrings
{
	{ L"NONE", LongText::NONE },
	{ L"HIT", LongText::HIT },
	{ L"MISS", LongText::MISS },
	{ L"CRIT", LongText::CRIT },
	{ L"GLANCING", LongText::GLANCING },
	{ L"BLOCK", LongText::BLOCK }
};

struct PrintText
{
public:
	std::wstring Text{};
	CoordStruct Location = CoordStruct::Empty;
	Point2D Offset = Point2D::Empty;
	int Duration = 15;
	PrintTextData Data{};

	PrintText(std::wstring text, CoordStruct location, Point2D offset, int duration, PrintTextData data)
	{
		this->Text = text;
		this->Location = location;
		this->Offset = offset;
		this->Duration = duration;
		this->_lifeTimer.Start(Duration);
		this->Data = data;
	}

	virtual bool CanPrintAndGetPos(RectangleStruct bound, Point2D& pos)
	{
		pos = ToClientPos(Location);
		// 视野内且不在迷雾下
		return _lifeTimer.InProgress() && InRect(pos, bound) && !InFog(Location);
	}
private:
	CDTimerClass _lifeTimer;
};

struct RollingText : public PrintText
{
public:
	int RollSpeed;

	RollingText(std::wstring text, CoordStruct location, Point2D offset, int rollSpeed, int duration, PrintTextData data) : PrintText(text, location, offset, duration, data)
	{
		this->RollSpeed = rollSpeed;
	}

	virtual bool CanPrintAndGetPos(RectangleStruct bound, Point2D& pos) override
	{
		if (PrintText::CanPrintAndGetPos(bound, pos))
		{
			this->Offset.Y -= RollSpeed;
			return true;
		}
		return false;
	}

};
