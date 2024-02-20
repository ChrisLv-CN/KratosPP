#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/EffectType/Effect/EffectData.h>
#include <Ext/Helper/MathEx.h>


class FireSuperEntity
{
public:
	bool Enable = false;

	std::vector<std::string> Supers{};
	std::vector<double> Chances{};

	int InitDelay = 0;
	Point2D RandomInitDelay = Point2D::Empty;
	int Delay = 0;
	Point2D RandomDelay = Point2D::Empty;
	int LaunchCount = 1;
	bool RealLaunch = false;

	int WeaponIndex = -1;
	bool ToTarget = true;

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Supers = reader->GetList(title + "Types", Supers);
		Chances = reader->GetChanceList(title + "Chances", Chances);

		InitDelay = reader->Get(title + "InitDelay", InitDelay);
		RandomInitDelay = reader->Get(title + "RandomInitDelay", RandomInitDelay);
		Delay = reader->Get(title + "Delay", Delay);
		RandomDelay = reader->Get(title + "RandomDelay", RandomDelay);
		LaunchCount = reader->Get(title + "LaunchCount", LaunchCount);
		RealLaunch = reader->Get(title + "RealLaunch", RealLaunch);
		WeaponIndex = reader->Get(title + "Weapon", WeaponIndex);
		ToTarget = reader->Get(title + "ToTarget", ToTarget);

		Enable = !Supers.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Enable)
			.Process(this->Supers)
			.Process(this->Chances)
			.Process(this->InitDelay)
			.Process(this->RandomInitDelay)
			.Process(this->Delay)
			.Process(this->RandomDelay)
			.Process(this->LaunchCount)
			.Process(this->RealLaunch)
			.Process(this->WeaponIndex)
			.Process(this->ToTarget)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		return const_cast<FireSuperEntity*>(this)->Serialize(stream);
	}
#pragma endregion
};

class FireSuperData : public EffectData
{
public:
	EFFECT_DATA(FireSuper);

	FireSuperEntity Data{};
	FireSuperEntity EliteData{};

	virtual void Read(INIBufferReader* reader) override
	{
		EffectData::Read(reader, TITLE);

		FireSuperEntity data;
		data.Read(reader, TITLE);
		if (!data.Supers.empty())
		{
			Data = data;
			EliteData = Data;
		}

		FireSuperEntity eliteData;
		eliteData.Read(reader, TITLE + "Elite");
		if (!eliteData.Supers.empty())
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
		return const_cast<FireSuperData*>(this)->Serialize(stream);
	}
#pragma endregion

protected:
	inline static std::string TITLE = "FireSuperWeapon.";
};
