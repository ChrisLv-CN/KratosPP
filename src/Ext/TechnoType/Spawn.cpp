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
		int size = GetSpawnData()->Spawns.size();
		if (i < size)
		{
			newId = GetSpawnData()->Spawns[i];
		}
		return IsNotNone(newId);
	}

	return false;
}

void Spawn::OnUpdate()
{
	if (!pTechno->SpawnManager)
	{
		Disable();
	}
}
