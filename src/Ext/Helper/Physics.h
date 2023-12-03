#pragma once
#include <string>
#include <map>
#include <vector>

#include <CellClass.h>
#include <MapClass.h>
#include <BuildingClass.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

static std::map<std::string, LandType> LandTypeStrings
{
	{ "Clear", LandType::Clear },
	{ "Road", LandType::Road },
	{ "Water", LandType::Water },
	{ "Rock", LandType::Rock },
	{ "Wall", LandType::Wall },
	{ "Tiberium", LandType::Tiberium },
	{ "Beach", LandType::Beach },
	{ "Rough", LandType::Rough },
	{ "Ice", LandType::Ice },
	{ "Railroad", LandType::Railroad },
	{ "Tunnel", LandType::Tunnel },
	{ "Weeds", LandType::Weeds }
};

template <>
inline bool Parser<LandType>::TryParse(const char* pValue, LandType* outValue)
{
	std::string key = pValue;
	auto it = LandTypeStrings.find(key);
	if (it != LandTypeStrings.end())
	{
		*outValue = it->second;
		return true;
	}
	return false;
}

static std::map<std::string, TileType> TileTypeStrings
{
	{ "Tunnel", TileType::Tunnel },
	{ "Water", TileType::Water },
	{ "Blank", TileType::Blank },
	{ "Ramp", TileType::Ramp },
	{ "Cliff", TileType::Cliff },
	{ "Shore", TileType::Shore },
	{ "Wet", TileType::Wet },
	{ "MiscPave", TileType::MiscPave },
	{ "Pave", TileType::Pave },
	{ "DirtRoad", TileType::DirtRoad },
	{ "PavedRoad", TileType::PavedRoad },
	{ "PavedRoadEnd", TileType::PavedRoadEnd },
	{ "PavedRoadSlope", TileType::PavedRoadSlope },
	{ "Median", TileType::Median },
	{ "Bridge", TileType::Bridge },
	{ "WoodBridge", TileType::WoodBridge },
	{ "ClearToSandLAT", TileType::ClearToSandLAT },
	{ "Green", TileType::Green },
	{ "NotWater", TileType::NotWater },
	{ "DestroyableCliff", TileType::DestroyableCliff }
};

template <>
inline bool Parser<TileType>::TryParse(const char* pValue, TileType* outValue)
{
	std::string key = pValue;
	auto it = TileTypeStrings.find(key);
	if (it != TileTypeStrings.end())
	{
		*outValue = it->second;
		return true;
	}
	return false;
}

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
