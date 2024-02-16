#include "Spawn.h"

#include <RocketLocomotionClass.h>

#include <Ext/Helper/FLH.h>

SpawnData* Spawn::GetSpawnData()
{
	if (!_spawnData)
	{
		_spawnData = INI::GetConfig<SpawnData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _spawnData;
}

bool Spawn::TryGetSpawnType(int i, std::string& newId)
{
	if (GetSpawnData()->MultiSpawns)
	{
		int size = GetSpawnData()->Spwans.size();
		if (i < size)
		{
			newId = GetSpawnData()->Spwans[i];
		}
		return IsNotNone(newId);
	}

	return false;
}

void Spawn::Awake()
{
	if (!pTechno->SpawnManager)
	{
		Disable();
	}
}
