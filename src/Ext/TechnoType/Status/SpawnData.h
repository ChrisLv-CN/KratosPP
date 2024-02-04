#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class SpawnData : public INIConfig
{
public:
	bool MultiSpawns = false;
	std::vector<std::string> Spwans{};

	int SpawnDelay = -1;

	bool NoShadowSpawnAlt = false;

	bool SpawnFireOnce = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Spwans = reader->GetList("Spawns", Spwans);
		MultiSpawns = !Spwans.empty();

		SpawnDelay = reader->Get("SpawnDelay", SpawnDelay);

		NoShadowSpawnAlt = reader->Get("NoShadowSpawnAlt", NoShadowSpawnAlt);

		SpawnFireOnce = reader->Get("SpawnFireOnce", SpawnFireOnce);
	}

};
