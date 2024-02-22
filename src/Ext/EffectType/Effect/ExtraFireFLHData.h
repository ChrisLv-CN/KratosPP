#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>


class ExtraFireFLHEntity
{
public:
	bool Enable = false;

	CoordStruct PrimaryFLH = CoordStruct::Empty;
	CoordStruct SecondaryFLH = CoordStruct::Empty;
	std::map<int, CoordStruct> WeaponXFLH{};

	bool PrimaryOnBody = false;
	bool SecondaryOnBody = false;
	std::vector<int> OnBodyIndexs{};

	bool PrimaryOnTarget = false;
	bool SecondaryOnTarget = false;
	std::vector<int> OnTargetIndexs{};

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		PrimaryFLH = reader->Get(title + "PrimaryFLH", PrimaryFLH);
		SecondaryFLH = reader->Get(title + "SecondaryFLH", SecondaryFLH);

		PrimaryOnBody = reader->Get(title + "PrimaryOnBody", PrimaryOnBody);
		SecondaryOnBody = reader->Get(title + "SecondaryOnBody", SecondaryOnBody);

		PrimaryOnTarget = reader->Get(title + "PrimaryOnTarget", PrimaryOnTarget);
		SecondaryOnTarget = reader->Get(title + "SecondaryOnTarget", SecondaryOnTarget);

		for (int i = 0; i < 128; i++)
		{
			CoordStruct flhs{};
			flhs = reader->Get(title + "Weapon" + std::to_string(i) + "FLH", flhs);
			if (!flhs.IsEmpty())
			{
				WeaponXFLH[i] = flhs;
			}
			if (reader->Get(title + "Weapon" + std::to_string(i) + "OnBody", false))
			{
				OnBodyIndexs.push_back(i);
			}
			if (reader->Get(title + "Weapon" + std::to_string(i) + "OnTarget", false))
			{
				OnTargetIndexs.push_back(i);
			}
		}

		Enable = !PrimaryFLH.IsEmpty() || !SecondaryFLH.IsEmpty() || !WeaponXFLH.empty();

	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->PrimaryFLH)
			.Process(this->SecondaryFLH)
			.Process(this->PrimaryOnBody)
			.Process(this->SecondaryOnBody)
			.Process(this->PrimaryOnTarget)
			.Process(this->SecondaryOnTarget)
			.Process(this->WeaponXFLH)
			.Process(this->OnBodyIndexs)
			.Process(this->OnTargetIndexs)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<ExtraFireFLHEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class ExtraFireFLHData : public EffectData
{
public:
	EFFECT_DATA(ExtraFire);

	ExtraFireFLHEntity Data{};
	ExtraFireFLHEntity EliteData{};

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "ExtraFire.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		ExtraFireFLHEntity data;
		data.Read(reader, title);
		if (data.Enable)
		{
			Data = data;
			EliteData = data;
		}

		ExtraFireFLHEntity eliteData;
		eliteData.Read(reader, title + "Elite");
		if (eliteData.Enable)
		{
			EliteData = eliteData;
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
		return const_cast<ExtraFireFLHData*>(this)->Serialize(stream);
	}
#pragma endregion
};
