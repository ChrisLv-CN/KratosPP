#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

enum class PrintTextAlign : int
{
	LEFT = 0, CENTER = 1, RIGHT = 2
};

template <>
inline bool Parser<PrintTextAlign>::TryParse(const char* pValue, PrintTextAlign* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'C':
		if (outValue)
		{
			*outValue = PrintTextAlign::CENTER;
		}
		return true;
	case 'R':
		if (outValue)
		{
			*outValue = PrintTextAlign::RIGHT;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = PrintTextAlign::LEFT;
		}
		return true;
	}
}

enum class SHPDrawStyle : int
{
	NUMBER = 0,
	TEXT = 1, // 固定帧
	PROGRESS = 2 // 进度条
};

template <>
inline bool Parser<SHPDrawStyle>::TryParse(const char* pValue, SHPDrawStyle* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'T':
		if (outValue)
		{
			*outValue = SHPDrawStyle::TEXT;
		}
		return true;
	case 'P':
		if (outValue)
		{
			*outValue = SHPDrawStyle::PROGRESS;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = SHPDrawStyle::NUMBER;
		}
		return true;
	}
}

class PrintTextData : public INIConfig
{
public:
	Point2D Offset{ 0, 0 };
	Point2D ShadowOffset{ 1, 1 };
	ColorStruct Color{ 252, 252, 252 };
	ColorStruct ShadowColor{ 82,85,82 };
	bool IsHouseColor = false;

	PrintTextAlign Align = PrintTextAlign::LEFT;

	bool UseSHP = false;
	SHPDrawStyle SHPDrawStyle = SHPDrawStyle::NUMBER; // 使用哪个帧来渲染
	std::string SHPFileName = "pipsnum.shp";
	int ZeroFrameIndex = 0;
	int MaxFrameIndex = -1;
	Point2D ImageSize{ 5, 8 };
	int Wrap = 1;

	bool NoNumbers = false; // 不使用数字
	// long text
	std::string HitSHP = "pipstext.shp";
	int HitIndex = 0;
	std::string MissSHP = "pipstext.shp";
	int MissIndex = 2;
	std::string CritSHP = "pipstext.shp";
	int CritIndex = 3;
	std::string GlancingSHP = "pipstext.shp";
	int GlancingIndex = 4;
	std::string BlockSHP = "pipstext.shp";
	int BlockIndex = 5;

	virtual void Read(INIBufferReader* ini) override
	{ }

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Offset = reader->Get(title + "Offset", Offset);
		ShadowOffset = reader->Get(title + "ShadowOffset", ShadowOffset);
		Color = reader->Get(title + "Color", Color);
		ShadowColor = reader->Get(title + "ShadowColor", ShadowColor);
		IsHouseColor = reader->Get(title + "IsHouseColor", IsHouseColor);

		Align = reader->Get(title + "Align", Align);

		UseSHP = reader->Get(title + "UseSHP", UseSHP);
		SHPFileName = reader->Get(title + "SHP", SHPFileName);
		ZeroFrameIndex = reader->Get(title + "ZeroFrameIndex", ZeroFrameIndex);
		MaxFrameIndex = reader->Get(title + "MaxFrameIndex", MaxFrameIndex);
		ImageSize = reader->Get(title + "ImageSize", ImageSize);
		Wrap = reader->Get(title + "Wrap", Wrap);

		NoNumbers = reader->Get(title + "NoNumbers", NoNumbers);
		// long text
		HitSHP = reader->Get(title + "HIT.SHP", HitSHP);
		HitIndex = reader->Get(title + "HIT.Index", HitIndex);
		MissSHP = reader->Get(title + "MISS.SHP", MissSHP);
		MissIndex = reader->Get(title + "MISS.Index", MissIndex);
		CritSHP = reader->Get(title + "CRIT.SHP", CritSHP);
		CritIndex = reader->Get(title + "CRIT.Index", CritIndex);
		GlancingSHP = reader->Get(title + "GLANCING.SHP", GlancingSHP);
		GlancingIndex = reader->Get(title + "GLANCING.Index", GlancingIndex);
		BlockSHP = reader->Get(title + "BLOCK.SHP", BlockSHP);
		BlockIndex = reader->Get(title + "BLOCK.Index", BlockIndex);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Offset)
			.Process(this->ShadowOffset)
			.Process(this->Color)
			.Process(this->ShadowColor)
			.Process(this->IsHouseColor)

			.Process(this->Align)

			.Process(this->UseSHP)
			.Process(this->SHPDrawStyle)
			.Process(this->SHPFileName)
			.Process(this->ZeroFrameIndex)
			.Process(this->MaxFrameIndex)
			.Process(this->ImageSize)
			.Process(this->Wrap)

			.Process(this->NoNumbers)
			.Process(this->HitSHP)
			.Process(this->HitIndex)
			.Process(this->MissSHP)
			.Process(this->MissIndex)
			.Process(this->CritSHP)
			.Process(this->CritIndex)
			.Process(this->GlancingSHP)
			.Process(this->GlancingIndex)
			.Process(this->BlockSHP)
			.Process(this->BlockIndex)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		INIConfig::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		INIConfig::Save(stream);
		return const_cast<PrintTextData*>(this)->Serialize(stream);
	}
#pragma endregion
};
