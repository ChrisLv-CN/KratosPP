#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <Ext/Helper.h>

#include <YRPP.h>

/**
 *@brief 尾巴绑定设置
 *
 */
class TrailData : public INIConfig
{
public:
	CoordStruct FLH = CoordStruct::Empty;
	bool IsOnTurret = false;

	bool CellLimit = false;
	std::vector<LandType> OnLandTypes{};
	std::vector<TileType> OnTileTypes{};

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
