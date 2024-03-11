#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>


class FireFLHEntity
{
public:
	bool Enable = false;

	bool PrimaryOnBody = false;
	bool SecondaryOnBody = false;
	std::vector<int> OnBodyIndexs{};

	bool PrimaryOnTarget = false;
	bool SecondaryOnTarget = false;
	std::vector<int> OnTargetIndexs{};

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		PrimaryOnBody = reader->Get(title + "PrimaryFireOnBody", PrimaryOnBody);
		SecondaryOnBody = reader->Get(title + "SecondaryFireOnBody", SecondaryOnBody);

		PrimaryOnTarget = reader->Get(title + "PrimaryOnTarget", PrimaryOnTarget);
		SecondaryOnTarget = reader->Get(title + "SecondaryOnTarget", SecondaryOnTarget);

		for (int i = 0; i < 128; i++)
		{
			std::string idx = std::to_string(i + 1);
			if (reader->Get(title + "Weapon" + idx + "OnBody", false))
			{
				OnBodyIndexs.push_back(i);
			}
			if (reader->Get(title + "Weapon" + idx + "OnTarget", false))
			{
				OnTargetIndexs.push_back(i);
			}
		}

		Enable = PrimaryOnBody || SecondaryOnBody || PrimaryOnTarget || SecondaryOnTarget || !OnBodyIndexs.empty() || !OnTargetIndexs.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->PrimaryOnBody)
			.Process(this->SecondaryOnBody)
			.Process(this->PrimaryOnTarget)
			.Process(this->SecondaryOnTarget)
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
		return const_cast<FireFLHEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class FireFLHData : public INIConfig
{
public:
	FireFLHEntity Data{};
	FireFLHEntity EliteData{};

	virtual void Read(INIBufferReader* reader) override
	{
		FireFLHEntity data;
		data.Read(reader, "");
		if (data.Enable)
		{
			Data = data;
			EliteData = data;
		}

		FireFLHEntity eliteData;
		eliteData.Read(reader, "Elite");
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
		INIConfig::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const override
	{
		INIConfig::Save(stream);
		return const_cast<FireFLHData*>(this)->Serialize(stream);
	}
#pragma endregion
};
