#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <TerrainClass.h>

#include <Common/INI/INI.h>

#include <Ext/Common/ExpandAnimsManager.h>

#include "TerrainDestroyAnimData.h"

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
