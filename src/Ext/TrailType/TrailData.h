#pragma once

#include <string>
#include <vector>

#include <GeneralDefinitions.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Helper/Physics.h>

enum class TrailDrawing : int
{
	BOTH = 0, LAND = 1, AIR = 2,
};

template <>
inline bool Parser<TrailDrawing>::TryParse(const char* pValue, TrailDrawing* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'L':
		if (outValue)
		{
			*outValue = TrailDrawing::LAND;
		}
		return true;
	case 'A':
		if (outValue)
		{
			*outValue = TrailDrawing::AIR;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = TrailDrawing::BOTH;
		}
		return true;
	}
}

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
	TrailDrawing DrawLevel = TrailDrawing::BOTH;

	virtual void Read(INIBufferReader* reader) override
	{ }

	void Read(INIBufferReader* reader, std::string title)
	{
		FLH = reader->Get(title + "FLH", FLH);
		IsOnTurret = reader->Get(title + "IsOn", IsOnTurret);

		OnLandTypes = reader->GetList<LandType>(title + "OnLands", OnLandTypes);
		OnTileTypes = reader->GetList<TileType>(title + "OnTiles", OnTileTypes);
		DrawLevel = reader->Get(title + "DrawLevel", DrawLevel);
		CellLimit = (!OnLandTypes.empty()) || (!OnTileTypes.empty());
	}
};
