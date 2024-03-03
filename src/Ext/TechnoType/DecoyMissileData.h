#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>

class DecoyMissileData : public INIConfig
{
public:
	std::string Weapon{ "" };
	std::string EliteWeapon{ "" };
	CoordStruct FLH = CoordStruct::Empty;
    CoordStruct Velocity = CoordStruct::Empty;
    int Delay = 4;
    int Life = 99999;
    bool AlwaysFire = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "DecoyMissile.");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		    Weapon = reader->Get(title + "Weapon", Weapon);
            EliteWeapon = reader->Get(title + "EliteWeapon", Weapon);

            FLH = reader->Get(title + "FLH", FLH);
            Velocity = reader->Get(title + "Velocity", Velocity);
            Delay = reader->Get(title + "Delay", Delay);
            Life = reader->Get(title + "Life", Life);
			AlwaysFire = reader->Get(title + "AlwaysFire", AlwaysFire);

            Enable = IsNotNone(Weapon) || IsNotNone(EliteWeapon);
	}

#pragma region save/load
	template <typename T>
	bool Serialize(T& stream)
	{
		return stream
			.Process(this->Weapon)
			.Process(this->EliteWeapon)
			.Process(this->FLH)
			.Process(this->Velocity)
			.Process(this->Delay)
			.Process(this->Life)
			.Process(this->AlwaysFire)
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
		return const_cast<DecoyMissileData*>(this)->Serialize(stream);
	}
#pragma endregion
private:

};

