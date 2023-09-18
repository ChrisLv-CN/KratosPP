#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <Ext/Helper.h>

#include <YRPP.h>

template <>
inline bool Parser<LandType>::TryParse(const char* pValue, LandType* outValue)
{
	return true;
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
	// TileType[] OnTileTypes;

	virtual void Read(INIBufferReader* reader) override
	{ }

	void Read(INIBufferReader* reader, std::string title)
	{
		FLH = reader->Get(title + "FLH", FLH);
		IsOnTurret = reader->Get(title + "IsOn", IsOnTurret);

		OnLandTypes = reader->GetList<LandType>(title + "OnLands", OnLandTypes);
		// OnTileTypes = reader->GetList<TileType>(title + "OnTiles", OnTileTypes);
		// CellLimit = (OnLandTypes.size() > 0) || (null != OnTileTypes && OnTileTypes.Any());
		CellLimit = (!OnLandTypes.empty());
	}
};
