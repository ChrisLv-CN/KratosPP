#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <TerrainClass.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

#include <Ext/Data/TerrainDestroyAnimData.h>
#include <Ext/ExpandAnimsManager.h>

namespace TerrainDestroyAnim
{
	static void PlayDestroyAnim(TerrainClass* pTerrain)
	{
		TerrainDestroyAnimData* data = INI::GetConfig<TerrainDestroyAnimData>(INI::Rules, pTerrain->Type->ID)->Data;
		if (data->Enable)
		{
			CoordStruct location = pTerrain->GetCoords();
			ExpandAnimsManager::PlayExpandAnims(*data, location);
		}
	}
};
