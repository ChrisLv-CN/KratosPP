#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/CommonStatus.h>

class AircraftDiveData : public INIConfig
{
public:

	double Distance = 10;

	int FlightLevel = 5 * Unsorted::LevelHeight;

	bool PullUpAfterFire = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Enable = reader->Get("Dive", Enable);

		Distance = reader->Get("Dive.Distance", Distance);

		FlightLevel = reader->Get("Dive.ForcePutOffset", FlightLevel);

		PullUpAfterFire = reader->Get("Dive.PullUpAfterFire", PullUpAfterFire);
	}
};
