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

	bool HeadToTarget = true;

	virtual void Read(INIBufferReader* reader) override
	{
		Enable = reader->Get("Dive", Enable);

		Distance = reader->Get("Dive.Distance", Distance);

		FlightLevel = reader->Get("Dive.FlightLevel", FlightLevel);

		PullUpAfterFire = reader->Get("Dive.PullUpAfterFire", PullUpAfterFire);

		HeadToTarget = reader->Get("Dive.HeadToTarget", HeadToTarget);
	}
};
