#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <WarheadTypeClass.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>

class VampireData : public EffectData
{
public:
	EFFECT_DATA(Vampire);

	double Chance = 1;

	double Percent = 0; // 吸血比例

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Vampire.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Chance = reader->GetChance(title + "Chance", Chance);

		Percent = reader->GetPercent(title + "Percent", Percent);

		Enable = Chance > 0 && Percent != 0;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Chance)

			.Process(this->Percent)
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
		return const_cast<VampireData*>(this)->Serialize(stream);
	}
#pragma endregion
};
