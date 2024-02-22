#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class DisableWeaponData : public EffectData
{
public:
	EFFECT_DATA(DisableWeapon);

	bool Disable = false;

	std::vector<LandType> OnLandTypes{};

	DisableWeaponData() : EffectData()
	{
		AffectWho = AffectWho::ALL;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader);
		Read(reader, "Weapon.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Disable = reader->Get(title + "Disable", false);
		OnLandTypes = reader->GetList(title + "DisableOnLands", OnLandTypes);

		Enable = Disable;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(Disable)
			.Process(OnLandTypes)
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
		return const_cast<DisableWeaponData*>(this)->Serialize(stream);
	}
#pragma endregion
};
