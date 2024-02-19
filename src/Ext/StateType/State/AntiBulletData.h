#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/CastEx.h>


class AntiBulletData : public EffectData
{
public:
	EFFECT_DATA(AntiBullet);

	int Weapon = -1;
	bool OneShotOneKill = true;
	bool Harmless = false;
	bool Self = true;
	bool ForPassengers = false;
	bool ScanAll = false;
	double Range = 0;
	double EliteRange = Range;
	int Rate = 0;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, TITLE);
	}

	virtual void Read(INIBufferReader* reader, std::string title) override
	{
		EffectData::Read(reader, title);

		Enable = reader->Get(title + "Enable", Enable);
		Weapon = reader->Get(title + "Weapon", Weapon);
		OneShotOneKill = reader->Get(title + "OneShotOneKill", OneShotOneKill);
		Harmless = reader->Get(title + "Harmless", Harmless);
		Self = reader->Get(title + "Self", Self);
		ForPassengers = reader->Get(title + "ForPassengers", ForPassengers);
		ScanAll = reader->Get(title + "ScanAll", ScanAll);
		Range = reader->Get(title + "Range", Range);
		EliteRange = reader->Get(title + "EliteRange", Range);
		Rate = reader->Get(title + "Rate", Rate);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Weapon)
			.Process(this->OneShotOneKill)
			.Process(this->Harmless)
			.Process(this->Self)
			.Process(this->ForPassengers)
			.Process(this->ScanAll)
			.Process(this->Range)
			.Process(this->EliteRange)
			.Process(this->Rate)
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
		return const_cast<AntiBulletData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
	inline static std::string TITLE = "AntiMissile.";
};
