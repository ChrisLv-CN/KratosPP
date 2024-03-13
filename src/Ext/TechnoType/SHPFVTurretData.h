#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>


class SHPFVTurretEntity
{
public:
	bool Enable = false;

    int WeaponTurretFrameIndex = -1;
	std::string WeaponTurretCustomSHP{ "" };

	virtual void Read(INIBufferReader* reader, int index)
	{
		std::string idx = std::to_string(index);
		WeaponTurretFrameIndex = reader->Get("WeaponTurretFrameIndex" + idx, WeaponTurretFrameIndex);
		WeaponTurretCustomSHP = reader->Get("WeaponTurretCustomSHP" + idx, WeaponTurretCustomSHP);

		Enable = WeaponTurretFrameIndex >= 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->WeaponTurretFrameIndex)
			.Process(this->WeaponTurretCustomSHP)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<SHPFVTurretEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class SHPFVTurretData : public INIConfig
{
public:
	std::map<int, SHPFVTurretEntity> Datas{};

	virtual void Read(INIBufferReader* reader) override
	{
		for (int i = 1; i <= 128; i++)
		{
			SHPFVTurretEntity data;
			data.Read(reader, i);
			if (data.Enable)
			{
				Datas[i] = data;
			}
		}

		Enable = !Datas.empty();
	}
#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Datas)
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
		return const_cast<SHPFVTurretData*>(this)->Serialize(stream);
	}
#pragma endregion
};
