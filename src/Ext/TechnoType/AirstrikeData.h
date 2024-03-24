#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/CommonStatus.h>

class AirstrikeData : public INIConfig
{
public:

	int AirstrikeTargetLaser = -1;
	ColorStruct AirstrikeTargetLaserColor = Colors::Red;

	bool AirstrikeDisableLine = false;
	bool AirstrikeDisableColor = false;
	bool AirstrikeDisableBlink = false;

	CoordStruct AirstrikePutOffset = CoordStruct::Empty;

	virtual void Read(INIBufferReader* reader) override
	{
		AirstrikeTargetLaser = reader->Get("AirstrikeTargetLaser", AirstrikeTargetLaser);
		if (AirstrikeTargetLaser >= 0)
		{
			AirstrikeTargetLaserColor = Drawing::Int_To_RGB(Add2RGB565(RulesClass::Instance->ColorAdd[AirstrikeTargetLaser]));
		}
		else
		{
			AirstrikeTargetLaserColor = Drawing::Int_To_RGB(Add2RGB565(RulesClass::Instance->ColorAdd[RulesClass::Instance->LaserTargetColor]));
		}
		AirstrikeTargetLaserColor = reader->Get("AirstrikeTargetLaserColor", AirstrikeTargetLaserColor);

		AirstrikeDisableLine = reader->Get("AirstrikeDisableLine", AirstrikeDisableLine);
		AirstrikeDisableColor = reader->Get("AirstrikeDisableColor", AirstrikeDisableColor);
		AirstrikeDisableBlink = reader->Get("AirstrikeDisableBlink", AirstrikeDisableBlink);

		AirstrikePutOffset = reader->Get("AirstrikePutOffset", AirstrikePutOffset);
	}
};
