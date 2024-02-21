#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>

class ImmuneData : public EffectData
{
public:
	EFFECT_DATA(Immune);

	bool Psionics; // 免疫心灵控制
	bool PsionicWeapons; // 免疫狂暴和心灵震爆
	bool Radiation; // 免疫辐射
	bool Poison; // 免疫病毒
	bool EMP; // 免疫EMP
	bool Parasite; // 免疫寄生
	bool Temporal; // 免疫超时空
	bool IsLocomotor; // 免疫磁电

	void CheckEnable()
	{
		Enable = Psionics || PsionicWeapons || Radiation || Poison || EMP || Parasite || Temporal || IsLocomotor;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Immune.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Psionics = reader->Get(title + "Psionics", Psionics);
		PsionicWeapons = reader->Get(title + "PsionicWeapons", PsionicWeapons);
		Radiation = reader->Get(title + "Radiation", Radiation);
		Poison = reader->Get(title + "Poison", Poison);
		EMP = reader->Get(title + "EMP", EMP);
		Parasite = reader->Get(title + "Parasite", Parasite);
		Temporal = reader->Get(title + "Temporal", Temporal);
		IsLocomotor = reader->Get(title + "IsLocomotor", IsLocomotor);

		CheckEnable();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Psionics)
			.Process(this->PsionicWeapons)
			.Process(this->Radiation)
			.Process(this->Poison)
			.Process(this->EMP)
			.Process(this->Parasite)
			.Process(this->Temporal)
			.Process(this->IsLocomotor)
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
		return const_cast<ImmuneData*>(this)->Serialize(stream);
	}
#pragma endregion

};
