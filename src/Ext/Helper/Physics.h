#pragma once
#include <string>
#include <map>
#include <vector>

#include <GeneralDefinitions.h>
#include <CellClass.h>
#include <MapClass.h>
#include <BuildingClass.h>

#include <Common/INI/INIConfig.h>

enum class PassError : int
{
	NONE = 0,
	PASS = 1, // 可通行
	UNDERGROUND = 2, // 潜地
	ONWATER = 3, // 掉水上
	HITWALL = 4, // 不可通行
	HITBUILDING = 5, // 撞建筑
	DOWNBRIDGE = 6, // 从上方撞桥
	UPBRIDEG = 7 // 从下方撞桥
};

static bool CanHit(BuildingClass* pBuilding, int targetZ, bool blade = false, int zOffset = 0)
{
	if (!blade)
	{
		int height = pBuilding->Type->Height;
		int sourceZ = pBuilding->GetCoords().Z;
		// Logger.Log($"Building Height {height}, {sourceZ + height * Game.LevelHeight + zOffset}");
		return targetZ <= (sourceZ + height * Unsorted::LevelHeight + zOffset);
	}
	return blade;
}

static PassError CanMoveTo(CoordStruct sourcePos, CoordStruct nextPos, bool passBuilding, CoordStruct& nextCellPos, bool& onBridge)
{
	PassError canPass = PassError::PASS;
	nextCellPos = sourcePos;
	onBridge = false;
	int deltaZ = sourcePos.Z - nextPos.Z;
	// 检查地面
	if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(nextPos))
	{
		nextCellPos = pTargetCell->GetCoordsWithBridge();
		onBridge = pTargetCell->ContainsBridge();
		if (nextCellPos.Z >= nextPos.Z)
		{
			// 沉入地面
			nextPos.Z = nextCellPos.Z;
			canPass = PassError::UNDERGROUND;
			// 检查悬崖
			switch (pTargetCell->GetTileType())
			{
			case TileType::Cliff:
			case TileType::DestroyableCliff:
				// 悬崖上可以往悬崖下移动
				if (deltaZ <= 0)
				{
					canPass = PassError::HITWALL;
				}
				break;
			}
		}
		// 检查桥
		if (canPass == PassError::UNDERGROUND && onBridge)
		{
			int bridgeHeight = nextCellPos.Z;
			if (sourcePos.Z > bridgeHeight && nextPos.Z <= bridgeHeight)
			{
				// 桥上砸桥下
				canPass = PassError::DOWNBRIDGE;
			}
			else if (sourcePos.Z < bridgeHeight && nextPos.Z >= bridgeHeight)
			{
				// 桥下穿桥上
				canPass = PassError::UPBRIDEG;
			}
		}
		// 检查建筑
		if (!passBuilding)
		{
			BuildingClass* pBuilding = pTargetCell->GetBuilding();
			if (pBuilding)
			{
				if (CanHit(pBuilding, nextPos.Z))
				{
					canPass = PassError::HITBUILDING;
				}
			}
		}
	}
	return canPass;
}
