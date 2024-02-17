#pragma once

#include <string>
#include <vector>

#include <Ext/Common/PrintTextData.h>
#include "EffectData.h"


enum class InfoMode : int
{
	NONE = 0, TEXT = 1, SHP = 2, IMAGE = 3,
};

template <>
inline bool Parser<InfoMode>::TryParse(const char* pValue, InfoMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'T':
		if (outValue)
		{
			*outValue = InfoMode::TEXT;
		}
		return true;
		{
	case 'S':
		if (outValue)
		{
			*outValue = InfoMode::SHP;
		}
		return true;
	case 'I':
		if (outValue)
		{
			*outValue = InfoMode::IMAGE;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = InfoMode::NONE;
		}
		return true;
		}
	}
}

enum class SortType : int
{
	FIRST = 0, MIN = 1, MAX = 2,
};

template <>
inline bool Parser<SortType>::TryParse(const char* pValue, SortType* outValue)
{
	pValue += 1;
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'I': // MIN
		if (outValue)
		{
			*outValue = SortType::MIN;
		}
		return true;
		{
	case 'A': // MAX
		if (outValue)
		{
			*outValue = SortType::MAX;
		}
		return true;
	default:
		if (outValue)
		{
			*outValue = SortType::FIRST;
		}
		return true;
		}
	}
}

class InfoEntity : public PrintTextData
{
public:
	std::string Watch{ "" };

	InfoMode Mode = InfoMode::NONE;
	bool ShowEnemy = true;
	bool OnlySelected = false;

	SortType Sort = SortType::FIRST;

	InfoEntity() : PrintTextData()
	{
		Align = PrintTextAlign::CENTER;
		Color = { 0, 252, 0 };
	}

	void Read(INIBufferReader* reader, std::string title)
	{
		Read(reader, title, Watch);
	}

	void Read(INIBufferReader* reader, std::string title, std::string watch)
	{
		PrintTextData::Read(reader, title);

		Watch = reader->Get(title + "Watch", watch); // 默认值由外部传入
		Mode = reader->Get(title + "Mode", Mode);
		switch (Mode)
		{
		case InfoMode::SHP:
			UseSHP = true;
			break;
		case InfoMode::IMAGE:
			UseSHP = true;
			SHPDrawStyle = SHPDrawStyle::PROGRESS;
			break;
		}
		ShowEnemy = reader->Get(title + "ShowEnemy", ShowEnemy);
		OnlySelected = reader->Get(title + "OnlySelected", OnlySelected);

		Sort = reader->Get(title + "Sort", Sort);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Watch)
			.Process(this->Mode)
			.Process(this->ShowEnemy)
			.Process(this->OnlySelected)
			.Process(this->Sort)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<InfoEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class InfoData : public EffectData
{
public:
	std::string ScriptName = "Info";

	InfoEntity Duration{};
	InfoEntity Delay{};
	InfoEntity InitDelay{};
	InfoEntity Stack{};

	InfoEntity Health{};
	InfoEntity Ammo{};
	InfoEntity Reload{};
	InfoEntity ROF{};

	InfoEntity ID{};
	InfoEntity Armor{};
	InfoEntity Mission{};

	InfoEntity Target{};
	InfoEntity Dest{};
	InfoEntity Location{};
	InfoEntity Cell{};
	InfoEntity BodyDir{};
	InfoEntity TurretDir{};

	InfoData() : EffectData()
	{
		Target.Color = { 252, 0, 0 };
		Dest.Color = { 252, 0, 0 };
		Location.Color = { 0, 252, 0 };
		Cell.Color = { 0, 252, 0 };
		BodyDir.Color = { 0, 252, 0 };
		TurretDir.Color = { 0, 0, 252 };
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Info.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{

		EffectData::Read(reader, title);

		std::string watch = reader->Get<std::string>(title + "Watch", "");
		if (IsNotNone(watch))
		{

			Duration.Read(reader, title + "Duration.", watch);
			Delay.Read(reader, title + "Delay.", watch);
			InitDelay.Read(reader, title + "InitDelay.", watch);
			Stack.Read(reader, title + "Stack.", watch);

			Health.Read(reader, title + "Health.", watch);
			Ammo.Read(reader, title + "Ammo.", watch);
			Reload.Read(reader, title + "Reload.", watch);
			ROF.Read(reader, title + "ROF.", watch);

			ID.Read(reader, title + "ID.", watch);
			Armor.Read(reader, title + "Armor.", watch);
			Mission.Read(reader, title + "Mission.", watch);

			Target.Read(reader, title + "Target.", watch);
			Dest.Read(reader, title + "Dest.", watch);
			Location.Read(reader, title + "Location.", watch);
			Cell.Read(reader, title + "Cell.", watch);
			BodyDir.Read(reader, title + "BodyDir.", watch);
			TurretDir.Read(reader, title + "TurretDir.", watch);

			Enable = Duration.Mode != InfoMode::NONE
				|| Delay.Mode != InfoMode::NONE
				|| InitDelay.Mode != InfoMode::NONE
				|| Stack.Mode != InfoMode::NONE

				|| Health.Mode != InfoMode::NONE
				|| Ammo.Mode != InfoMode::NONE
				|| Reload.Mode != InfoMode::NONE
				|| ROF.Mode != InfoMode::NONE

				|| ID.Mode != InfoMode::NONE
				|| Armor.Mode != InfoMode::NONE
				|| Mission.Mode != InfoMode::NONE

				|| Target.Mode != InfoMode::NONE
				|| Dest.Mode != InfoMode::NONE
				|| Location.Mode != InfoMode::NONE
				|| Cell.Mode != InfoMode::NONE
				|| BodyDir.Mode != InfoMode::NONE
				|| TurretDir.Mode != InfoMode::NONE
				;
		}
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Duration)
			.Process(this->Delay)
			.Process(this->InitDelay)
			.Process(this->Stack)

			.Process(this->Health)
			.Process(this->Ammo)
			.Process(this->Reload)
			.Process(this->ROF)

			.Process(this->ID)
			.Process(this->Armor)
			.Process(this->Mission)

			.Process(this->Target)
			.Process(this->Dest)
			.Process(this->Location)
			.Process(this->Cell)
			.Process(this->BodyDir)
			.Process(this->TurretDir)

			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange) override
	{
		EffectData::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		EffectData::Save(stream);
		return const_cast<InfoData*>(this)->Serialize(stream);
	}
#pragma endregion

};


