#include "../TechnoStatus.h"
#include <Ext/CommonStatus.h>

SpawnData* TechnoStatus::GetSpawnData()
{
	if (!_spawnData)
	{
		_spawnData = INI::GetConfig<SpawnData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _spawnData;
}

bool TechnoStatus::TryGetSpawnType(int i, std::string& newId)
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
