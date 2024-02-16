#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class MissileHomingData : public INIConfig
{
public:
	bool Homing = false;

	bool FacingTarget = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Homing = reader->Get("Missile.Homing", Homing);

		FacingTarget = reader->Get("Missile.FacingTarget", FacingTarget);
	}

};
