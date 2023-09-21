#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Ext/PrintText/PrintTextData.h>

class DamageText : public PrintTextData
{
public:
	bool Hidden = false;
	bool Detail = true;
	int Rate = 0;
	Point2D XOffset{ -15, 15 };
	Point2D YOffset{ -12, 12 };
	int RollSpeed = 1;
	int Duration = 75;

	DamageText() : PrintTextData()
	{ }

	DamageText(bool isDamage) : PrintTextData()
	{
		if (isDamage)
		{
			Color = { 252, 0, 0 };
			ZeroFrameIndex = 30;
			HitIndex = 1;
		}
		else
		{
			Color = { 0, 252 ,0 };
			ZeroFrameIndex = 0;
			HitIndex = 0;
		}
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		PrintTextData::Read(reader, title);

		Hidden = reader->Get(title + "Hidden", Hidden);
		Detail = reader->Get(title + "Detail", Detail);
		Rate = reader->Get(title + "Rate", Rate);
		XOffset = reader->Get(title + "XOffset", XOffset);
		YOffset = reader->Get(title + "YOffset", YOffset);
		RollSpeed = reader->Get(title + "RollSpeed", RollSpeed);
		Duration = reader->Get(title + "Duration", Duration);
	}
};

class DamageTextData : public INIConfig
{
public:
	bool Hidden = false;

	DamageText Damage{ true };
	DamageText Repair{ false };

	virtual void Read(INIBufferReader* reader) override
	{
		int infDeath = reader->Get("InfDeath", 0);
		INIBufferReader* avReader = INI::GetSection(INI::Rules, INI::SectionAudioVisual);
		// 读取全局设置
		Read(avReader, TITLE);
		Read(avReader, TITLE + std::to_string(infDeath) + ".");
		// 读取个体设置
		Read(reader, TITLE);
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Hidden = reader->Get(title + "Hidden", Hidden);

		Damage.Read(reader, title);
		Damage.Read(reader, title + "Damage.");

		Repair.Read(reader, title);
		Repair.Read(reader, title + "Repair.");
	}
private:
	const std::string TITLE = "DamageText.";
};

struct DamageTextCache
{
	int StartFrame = 0;
	int Value = 0;

	DamageTextCache()
	{
		StartFrame = Unsorted::CurrentFrame;
	}

	DamageTextCache(int value) : DamageTextCache()
	{
		Value = value;
	}

	void Add(int value)
	{
		Value += value;
	}
};
