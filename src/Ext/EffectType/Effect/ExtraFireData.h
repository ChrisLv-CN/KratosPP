#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>

#include "ExtraFireFLHData.h"

class ExtraFireEntity
{
public:
	bool Enable = false;

	std::vector<std::string> Primary{};
	std::vector<std::string> Secondary{};

	std::map<int, std::vector<std::string>> WeaponX;

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Primary = reader->GetList(title + "Primary", Primary);
		ClearIfGetNone(Primary);
		Secondary = reader->GetList(title + "Secondary", Secondary);
		ClearIfGetNone(Secondary);

		for (int i = 0; i < 128; i++)
		{
			std::vector<std::string> weapons{};
			weapons = reader->GetList(title + "Weapon" + std::to_string(i), weapons);
			ClearIfGetNone(weapons);
			if (!weapons.empty())
			{
				WeaponX[i] = weapons;
			}
		}

		Enable = !Primary.empty() || !Secondary.empty() || !WeaponX.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Primary)
			.Process(this->Secondary)
			.Process(this->WeaponX)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<ExtraFireEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class ExtraFireData : public EffectData
{
public:
	EFFECT_DATA(ExtraFire);

	ExtraFireEntity Data{};
	ExtraFireEntity EliteData{};

	ExtraFireFLHEntity FLHData{};
	ExtraFireFLHEntity EliteFLHData{};

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "ExtraFire.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		ExtraFireEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Data = data;
		}

		ExtraFireEntity eliteData;
		eliteData.Read(reader, title + "Elite");
		if (eliteData.Enable)
		{
			EliteData = eliteData;
		}

		ExtraFireFLHEntity flhData;
		flhData.Read(reader, title);
		if (flhData.Enable)
		{
			FLHData = flhData;
		}

		ExtraFireFLHEntity eliteFLHData;
		eliteFLHData.Read(reader, title + "Elite");
		if (eliteFLHData.Enable)
		{
			EliteFLHData = eliteFLHData;
		}

		Enable = Data.Enable || EliteData.Enable;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)
			.Process(this->EliteData)
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
		return const_cast<ExtraFireData*>(this)->Serialize(stream);
	}
#pragma endregion
};
