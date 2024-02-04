#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class AutoFireAreaWeaponData : public INIConfig
{
public:
	int WeaponIndex = -1;
	int InitialDelay = 0;
	bool CheckAmmo = false;
	bool UseAmmo = false;
	bool TargetToGround = false;

	virtual void Read(INIBufferReader* reader) override
	{
		WeaponIndex = reader->Get("AutoFireAreaWeapon", WeaponIndex);
		Enable = WeaponIndex > -1 && WeaponIndex < 2;

		InitialDelay = reader->Get("AutoFireAreaWeapon.InitialDelay", InitialDelay);
		CheckAmmo = reader->Get("AutoFireAreaWeapon.CheckAmmo", CheckAmmo);
		UseAmmo = reader->Get("AutoFireAreaWeapon.UseAmmo", UseAmmo);
		TargetToGround = reader->Get("AutoFireAreaWeapon.TargetToGround", TargetToGround);
	}

};
