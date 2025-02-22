﻿#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/Helper/CastEx.h>

#include "EffectData.h"

class CrateBuffData : public EffectData
{
public:
	EFFECT_DATA(CrateBuff);

	double FirepowerMultiplier = 1.0;
	double ArmorMultiplier = 1.0;
	double SpeedMultiplier = 1.0;
	double ROFMultiplier = 1.0;
	bool Cloakable = false;
	bool ForceDecloak = false;
	double RangeMultiplier = 1.0;
	double RangeCell = 0.0;

	CrateBuffData() : EffectData()
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
		this->RangeMultiplier += a.RangeMultiplier;
		this->RangeCell += a.RangeCell;
		return *this;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Status.");
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		FirepowerMultiplier = reader->Get(title + "FirepowerMultiplier", FirepowerMultiplier);
		ArmorMultiplier = reader->Get(title + "ArmorMultiplier", ArmorMultiplier);
		SpeedMultiplier = reader->Get(title + "SpeedMultiplier", SpeedMultiplier);
		ROFMultiplier = reader->Get(title + "ROFMultiplier", ROFMultiplier);
		Cloakable = reader->Get(title + "Cloakable", Cloakable);
		ForceDecloak = reader->Get(title + "ForceDecloak", ForceDecloak);
		RangeMultiplier = reader->Get(title + "RangeMultiplier", RangeMultiplier);
		RangeCell = reader->Get(title + "RangeCell", RangeCell);

		Enable = FirepowerMultiplier != 1.0
			|| ArmorMultiplier != 1.0
			|| SpeedMultiplier != 1.0
			|| ROFMultiplier != 1.0
			|| Cloakable || ForceDecloak
			|| RangeMultiplier != 1.0
			|| RangeCell != 0.0;
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
			.Process(this->RangeMultiplier)
			.Process(this->RangeCell)
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

};
