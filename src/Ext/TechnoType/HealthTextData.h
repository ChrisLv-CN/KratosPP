#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Ext/PrintText/PrintTextData.h>

enum class HealthTextStyle
{
	AUTO = 0, FULL = 1, SHORT = 2, PERCENT = 3
};

template <>
inline bool Parser<HealthTextStyle>::TryParse(const char* pValue, HealthTextStyle* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'F':
		if (outValue)
		{
			*outValue = HealthTextStyle::FULL;
		}
		return true;
	case 'S':
		if (outValue)
		{
			*outValue = HealthTextStyle::SHORT;
		}
		return true;
	case 'P':
		if (outValue)
		{
			*outValue = HealthTextStyle::PERCENT;
		}
		return true;
	}
	return false;
}

class HealthTextEntity : public PrintTextData
{
public:
	bool Hidden = false;
	bool ShowEnemy = false;
	bool ShowHover = false;
	HealthTextStyle Style = HealthTextStyle::FULL;
	HealthTextStyle HoverStyle = HealthTextStyle::SHORT;

	HealthTextEntity() : PrintTextData()
	{
		Align = PrintTextAlign::LEFT;
		SHPFileName = "pipsnum.shp";
		ImageSize = { 5, 8 };
	}

	HealthTextEntity(HealthState healthState) : HealthTextEntity()
	{
		switch (healthState)
		{
		case HealthState::Green:
			Color = Colors::Green;
			ZeroFrameIndex = 0;
			break;
		case HealthState::Yellow:
			Color = Colors::Yellow;
			ZeroFrameIndex = 15;
			break;
		case HealthState::Red:
			Color = Colors::Red;
			ZeroFrameIndex = 30;
			break;
		}
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		PrintTextData::Read(reader, title);

		Hidden = reader->Get(title + "Hidden", Hidden);
		ShowEnemy = reader->Get(title + "ShowEnemy", ShowEnemy);
		ShowHover = reader->Get(title + "ShowHover", ShowHover);
		Style = reader->Get(title + "Style", Style);
		HoverStyle = reader->Get(title + "HoverStyle", HoverStyle);
	}
};

struct HealthTextData
{
public:
	inline static std::string TITLE = "DamageText.";

	bool IsRead = false;

	bool Hidden = false;

	HealthTextEntity Green{ HealthState::Green };
	HealthTextEntity Yellow{ HealthState::Yellow };
	HealthTextEntity Red{ HealthState::Red };

	HealthTextData()
	{ }

	HealthTextData(AbstractType absType) : HealthTextData()
	{
		switch (absType)
		{
		case AbstractType::Building:
			Green.Style = HealthTextStyle::FULL;
			Yellow.Style = HealthTextStyle::FULL;
			Red.Style = HealthTextStyle::FULL;
			break;
		case AbstractType::Infantry:
			Green.Style = HealthTextStyle::SHORT;
			Yellow.Style = HealthTextStyle::SHORT;
			Red.Style = HealthTextStyle::SHORT;
			break;
		case AbstractType::Unit:
			Green.Style = HealthTextStyle::FULL;
			Yellow.Style = HealthTextStyle::FULL;
			Red.Style = HealthTextStyle::FULL;
			break;
		case AbstractType::Aircraft:
			Green.Style = HealthTextStyle::FULL;
			Yellow.Style = HealthTextStyle::FULL;
			Red.Style = HealthTextStyle::FULL;
			break;
		}
	}

	void Read(INIBufferReader* reader, std::string title = TITLE)
	{
		Hidden = reader->Get(title + "Hidden", Hidden);
		// Phobos
		Hidden = reader->Get("HealthBar.Hide", Hidden);

		Green.Read(reader, title);
		Yellow.Read(reader, title);
		Red.Read(reader, title);

		Green.Read(reader, title + "Green.");
		Yellow.Read(reader, title + "Yellow.");
		Red.Read(reader, title + "Red.");

		IsRead = true;
	}
};

struct HealthTextControlData
{
	bool IsRead = false;

	bool Hidden = false;

	HealthTextData Building{ AbstractType::Building };
	HealthTextData Infantry{ AbstractType::Infantry };
	HealthTextData Unit{ AbstractType::Unit };
	HealthTextData Aircraft{ AbstractType::Aircraft };

	void Read()
	{
		INIBufferReader* avReader = INI::GetSection(INI::Rules, INI::SectionAudioVisual);

		Hidden = avReader->Get(HealthTextData::TITLE + "Hidden", false);

		Building.Read(avReader, HealthTextData::TITLE);
		Building.Read(avReader, HealthTextData::TITLE + "Building.");

		Infantry.Read(avReader, HealthTextData::TITLE);
		Infantry.Read(avReader, HealthTextData::TITLE + "Infantry.");

		Unit.Read(avReader, HealthTextData::TITLE);
		Unit.Read(avReader, HealthTextData::TITLE + "Unit.");

		Aircraft.Read(avReader, HealthTextData::TITLE);
		Aircraft.Read(avReader, HealthTextData::TITLE + "Aircraft.");

		IsRead = true;
	}
};

