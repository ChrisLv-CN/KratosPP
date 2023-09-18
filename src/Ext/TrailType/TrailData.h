#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <Ext/Helper.h>

#include <YRPP.h>


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
		outValue = &it->second;
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
		outValue = &it->second;
		return true;
	}
	return false;
}

/**
 *@brief 尾巴绑定设置
 *
 */
class TrailData : public INIConfig
{
public:
	CoordStruct FLH;
	bool IsOnTurret;

	bool CellLimit;
	std::vector<LandType> OnLandTypes;
	std::vector<TileType> OnTileTypes;

	virtual void Read(INIBufferReader* reader) override
	{ }

	void Read(INIBufferReader* reader, std::string title)
	{
		FLH = reader->Get(title + "FLH", FLH);
		IsOnTurret = reader->Get(title + "IsOn", IsOnTurret);

		OnLandTypes = reader->GetList<LandType>(title + "OnLands", OnLandTypes);
		OnTileTypes = reader->GetList<TileType>(title + "OnTiles", OnTileTypes);
		CellLimit = (!OnLandTypes.empty()) || (!OnTileTypes.empty());
	}
};
