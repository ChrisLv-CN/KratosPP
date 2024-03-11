#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class SupportSpawnsData : public INIConfig
{
public:
	std::vector<std::string> Weapons;
	std::vector<std::string> EliteWeapons;

	bool SwitchFLH = false;
	bool AlwaysFire = false;

	bool IsOnTurret = true;
	bool TurnTurret = false;


	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "SupportSpawns.");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Weapons = reader->GetList(title + "Weapons", Weapons);
		ClearIfGetNone(Weapons);
		EliteWeapons = reader->GetList(title + "EliteWeapons", Weapons);
		ClearIfGetNone(EliteWeapons);

		SwitchFLH = reader->Get(title + "SwitchFLH", SwitchFLH);
		AlwaysFire = reader->Get(title + "AlwaysFire", AlwaysFire);

		IsOnTurret = reader->Get(title + "IsOnTurret", IsOnTurret);
		TurnTurret = reader->Get(title + "TurnTurret", TurnTurret);

		Enable = !Weapons.empty() || !EliteWeapons.empty();
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Weapons)
			.Process(this->EliteWeapons)
			.Process(this->SwitchFLH)
			.Process(this->AlwaysFire)
			.Process(this->IsOnTurret)
			.Process(this->TurnTurret)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		INIConfig::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		INIConfig::Save(stream);
		return const_cast<SupportSpawnsData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};

class SupportSpawnsFLHData : public INIConfig
{
public:
	CoordStruct SupportWeaponFLH = CoordStruct::Empty;
	CoordStruct EliteSupportWeaponFLH = CoordStruct::Empty;

	CoordStruct SupportWeaponHitFLH = CoordStruct::Empty;
	CoordStruct EliteSupportWeaponHitFLH = CoordStruct::Empty;


	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		SupportWeaponFLH = reader->Get(title + "SupportWeaponFLH", SupportWeaponFLH);
		EliteSupportWeaponFLH = reader->Get(title + "EliteSupportWeaponFLH", SupportWeaponFLH);

		SupportWeaponHitFLH = reader->Get(title + "SupportWeaponHitFLH", SupportWeaponHitFLH);
		EliteSupportWeaponHitFLH = reader->Get(title + "EliteSupportWeaponHitFLH", SupportWeaponHitFLH);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->SupportWeaponFLH)
			.Process(this->EliteSupportWeaponFLH)
			.Process(this->SupportWeaponHitFLH)
			.Process(this->EliteSupportWeaponHitFLH)
			.Success();
	};

	virtual bool Load(ExStreamReader& stream, bool registerForChange)
	{
		INIConfig::Load(stream, registerForChange);
		return this->Serialize(stream);
	}
	virtual bool Save(ExStreamWriter& stream) const
	{
		INIConfig::Save(stream);
		return const_cast<SupportSpawnsFLHData*>(this)->Serialize(stream);
	}
#pragma endregion
private:
};
