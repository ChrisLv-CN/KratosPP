﻿#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/Data/EffectData.h>
#include <Ext/Helper/CastEx.h>

class CrateBuffData : public EffectData
{
public:
	double FirepowerMultiplier = 1.0;
	double ArmorMultiplier = 1.0;
	double SpeedMultiplier = 1.0;
	double ROFMultiplier = 1.0;
	bool Cloakable = false;
	bool ForceDecloak = false;

	CrateBuffData()
	{
		AffectWho = AffectWho::ALL;
	}

	//addition
	CrateBuffData& operator+(const CrateBuffData& a)
	{
		return *this += a;
	}

	CrateBuffData& operator+=(const CrateBuffData& a)
	{
		this->FirepowerMultiplier += a.FirepowerMultiplier;
		this->ArmorMultiplier += a.ArmorMultiplier;
		this->SpeedMultiplier += a.SpeedMultiplier;
		this->ROFMultiplier += a.ROFMultiplier;
		this->Cloakable = a.Cloakable;
		this->ForceDecloak = a.ForceDecloak;
		return *this;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader, TITLE);

		FirepowerMultiplier = reader->Get(TITLE + "FirepowerMultiplier", FirepowerMultiplier);
		ArmorMultiplier = reader->Get(TITLE + "ArmorMultiplier", ArmorMultiplier);
		SpeedMultiplier = reader->Get(TITLE + "SpeedMultiplier", SpeedMultiplier);
		ROFMultiplier = reader->Get(TITLE + "ROFMultiplier", ROFMultiplier);
		Cloakable = reader->Get(TITLE + "Cloakable", Cloakable);
		ForceDecloak = reader->Get(TITLE + "ForceDecloak", ForceDecloak);

		Enable = FirepowerMultiplier != 1.0 || ArmorMultiplier != 1.0 || SpeedMultiplier != 1.0 || ROFMultiplier != 1.0 || Cloakable || ForceDecloak;
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->FirepowerMultiplier)
			.Process(this->ArmorMultiplier)
			.Process(this->SpeedMultiplier)
			.Process(this->ROFMultiplier)
			.Process(this->Cloakable)
			.Process(this->ForceDecloak)
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
		return const_cast<CrateBuffData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "Status.";
};
