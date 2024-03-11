#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class OverrideWeaponEntity
{
public:
	bool Enable = false;

	std::vector<std::string> Types{};
	bool RandomType = false;
	std::vector<int> Weights{};
	int Index = -1;
	double Chance = 1;

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Types = reader->GetList(title + "Types", Types);
		ClearIfGetNone(Types);
		RandomType = Types.size() > 1;
		Weights = reader->GetList(title + "Weights", Weights);
		Index = reader->Get(title + "Index", Index);
		Chance = reader->GetChance(title + "Chance", Chance);

		Enable = !Types.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Types)
			.Process(this->RandomType)
			.Process(this->Weights)
			.Process(this->Index)
			.Process(this->Chance)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<OverrideWeaponEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class OverrideWeaponData : public EffectData
{
public:
	EFFECT_DATA(OverrideWeapon);

	OverrideWeaponEntity Data{};
	OverrideWeaponEntity EliteData{};

	bool UseToDeathWeapon = false;

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader);
		Read(reader, "OverrideWeapon.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);


		OverrideWeaponEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Data = data;
			EliteData = Data;
		}

		OverrideWeaponEntity eliteData;
		eliteData.Read(reader, title + "Elite");
		if (eliteData.Enable)
		{
			EliteData = eliteData;
		}

		UseToDeathWeapon = reader->Get(title + "UseToDeathWeapon", UseToDeathWeapon);

		Enable = Data.Enable || EliteData.Enable;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Data)
			.Process(this->EliteData)
			.Process(this->UseToDeathWeapon)
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
		return const_cast<OverrideWeaponData*>(this)->Serialize(stream);
	}
#pragma endregion
};
