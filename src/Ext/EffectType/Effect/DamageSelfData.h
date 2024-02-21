#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Helper/MathEx.h>

#include "EffectData.h"

class DamageSelfData : public EffectData
{
public:
	EFFECT_DATA(DamageSelf);

	int Damage = 0;
	int ROF = 0;
	std::string Warhead{ "" };
	bool WarheadAnim = false;
	bool Decloak = false;
	bool IgnoreArmor = true;
	bool FirepowerMultiplier = false;
	bool Peaceful = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "DamageSelf.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Damage = reader->Get(title + "Damage", Damage);
		ROF = reader->Get(title + "ROF", ROF);
		Warhead = reader->Get(title + "Warhead", Warhead);
		WarheadAnim = reader->Get(title + "WarheadAnim", WarheadAnim);
		Decloak = reader->Get(title + "Decloak", Decloak);
		IgnoreArmor = reader->Get(title + "IgnoreArmor", IgnoreArmor);
		FirepowerMultiplier = reader->Get(title + "FirepowerMultiplier", FirepowerMultiplier);
		Peaceful = reader->Get(title + "Peaceful", Peaceful);

		Enable = Damage != 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Damage)
			.Process(this->ROF)
			.Process(this->Warhead)
			.Process(this->WarheadAnim)
			.Process(this->Decloak)
			.Process(this->IgnoreArmor)
			.Process(this->FirepowerMultiplier)
			.Process(this->Peaceful)
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
		return const_cast<DamageSelfData*>(this)->Serialize(stream);
	}
#pragma endregion
};
