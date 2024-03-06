#pragma once
#include <string>
#include <map>
#include <vector>

#include <GeneralDefinitions.h>
#include <CellClass.h>
#include <MapClass.h>
#include <BuildingClass.h>
#include <InfantryClass.h>

#include <Common/INI/INIConfig.h>

#include "Status.h"

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

static bool FallingDown(TechnoClass* pTechno, int fallingDestroyHeight, bool hasParachute)
{
	TechnoTypeClass* pType = pTechno->GetTechnoType();
	// 检查是否在悬崖上摔死
	bool canPass = true;
	bool isWater = false;
	CoordStruct location = pTechno->GetCoords();
	CoordStruct targetPos = location;
	CellClass* pCell = MapClass::Instance->TryGetCellAt(location);
	if (pCell)
	{
		CoordStruct cellPos = pCell->GetCoordsWithBridge();
		pTechno->OnBridge = pCell->ContainsBridge();

		if (cellPos.Z >= location.Z)
		{
			targetPos.Z = cellPos.Z;
			pTechno->SetLocation(targetPos);
		}
		// 当前格子所在的位置不可通行，炸了它
		canPass = pCell->IsClearToMove(pType->SpeedType, pType->MovementZone, true, true);
		if (canPass && pCell->GetBuilding() != nullptr)
		{
			canPass = false;
		}
		if (!canPass)
		{
			isWater = pCell->Tile_Is_Water();
		}

	}
	int height = pTechno->GetHeight();
	bool inAir = height >= Unsorted::LevelHeight * 2;
	if (inAir && pType->ConsideredAircraft)
	{
		// 飞行器在天上，免死
		pTechno->SetDestination(pCell, true);
		if (pTechno->Target)
		{
			pTechno->QueueMission(Mission::Attack, false);
		}
		else
		{
			if (pTechno->WhatAmI() == AbstractType::Aircraft)
			{
				pTechno->QueueMission(Mission::Enter, false);
			}
			else
			{
				pTechno->QueueMission(Mission::Guard, false);
			}
		}
	}
	else
	{
		bool drop = false;
		bool sinking = false;
		// 检查下方是不是水
		if (isWater)
		{
			LocoType locoType = GetLocoType(pTechno);
			switch (locoType)
			{
			case LocoType::Hover:
			case LocoType::Ship:
				// 船和悬浮不下沉
				canPass = true;
				break;
			case LocoType::Jumpjet:
				if (!pType->BalloonHover)
				{
					sinking = true;
				}
				break;
			default:
				sinking = true;
				break;
			}
		}
		// 高度大于一定值时强制摔死
		if (fallingDestroyHeight > 0 && height >= fallingDestroyHeight)
		{
			canPass = false;
		}
		if (canPass)
		{
			if (height > 0)
			{
				// 离地
				pTechno->IsFallingDown = true;
				drop = true;
			}
			else
			{
				// 贴地
				pTechno->Scatter(targetPos, true, true);
			}
		}
		else
		{
			// 摔死
			if (height <= 0 && sinking)
			{
				pTechno->IsSinking = true;
			}
			else
			{
				pTechno->DropAsBomb();
				drop = true;
			}
		}
		if (drop)
		{
			if (hasParachute && inAir)
			{
				// ObjectClass.SpawnParachuted(Coords)需要检查单位Unlimbo成功，此处手动添加降落伞动画
				if (AnimTypeClass* pAnimType = RulesClass::Instance->Parachute)
				{
					CoordStruct parachutePos = targetPos;
					parachutePos.Z += 75;
					AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, parachutePos);
					pTechno->Parachute = pAnim;
					pAnim->SetOwnerObject(pTechno);
					pAnim->Owner = pTechno->Owner;
				}
			}
			else if (pTechno->WhatAmI() == AbstractType::Infantry)
			{
				dynamic_cast<InfantryClass*>(pTechno)->PlayAnim(Sequence::Paradrop);
			}
		}
	}
	return !canPass;
}
