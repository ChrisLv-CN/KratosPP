#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

enum class BuildingRangeMode
{
	NONE = 0, LINE = 1, CELL = 2, SHP = 3
};

template <>
inline bool Parser<BuildingRangeMode>::TryParse(const char* pValue, BuildingRangeMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'L':
		if (outValue)
		{
			*outValue = BuildingRangeMode::LINE;
		}
		return true;
	case 'C':
		if (outValue)
		{
			*outValue = BuildingRangeMode::CELL;
		}
		return true;
	case 'S':
		if (outValue)
		{
			*outValue = BuildingRangeMode::SHP;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = BuildingRangeMode::NONE;
		}
	}
	return false;
}

class BuildingRangeData : public INIConfig
{
public:
	BuildingRangeMode Mode = BuildingRangeMode::NONE; // 用什么方式显示建造范围
	ColorStruct Color = Colors::White; // 显示线条的颜色
	bool Dashed = true; // 虚线
	std::string SHPFileName = "placerange.shp"; // shp文件名
	int ZeroFrameIndex = 0; // 平面的起始帧序号

	virtual void Read(INIBufferReader* reader) override
	{
		// 读全局
		INIBufferReader* avReader = INI::GetSection(INI::Rules, INI::SectionAudioVisual);
		Mode = avReader->Get(TITLE + "Mode", Mode);
		Color = avReader->Get(TITLE + "Color", Color);
		Dashed = avReader->Get(TITLE + "Dashed", Dashed);
		SHPFileName = avReader->Get(TITLE + "SHP", SHPFileName);
		ZeroFrameIndex = avReader->Get(TITLE + "ZeroFrameIndex", ZeroFrameIndex);

		// 读个体
		Mode = reader->Get(TITLE + "Mode", Mode);
		Color = reader->Get(TITLE + "Color", Color);
		Dashed = reader->Get(TITLE + "Dashed", Dashed);
		SHPFileName = reader->Get(TITLE + "SHP", SHPFileName);
		ZeroFrameIndex = reader->Get(TITLE + "ZeroFrameIndex", ZeroFrameIndex);
	}
private:
	inline static std::string TITLE = "BuildingRange.";
};
