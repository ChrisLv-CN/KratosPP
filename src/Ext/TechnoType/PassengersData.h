#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class PassengersData : public INIConfig
{
public:
	bool PassiveAcquire = true;
	bool ForceFire = false;
	bool MobileFire = true;
	bool SameFire = true;

	virtual void Read(INIBufferReader* reader) override
	{
		PassiveAcquire = reader->Get("Passengers.PassiveAcquire", PassiveAcquire);
		ForceFire = reader->Get("Passengers.ForceFire", ForceFire);
		MobileFire = reader->Get("Passengers.MobileFire", MobileFire);
		SameFire = reader->Get("Passengers.SameFire", SameFire);
	}

};
