#pragma once
#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/CommonStatus.h>

class AircraftGuardData : public INIConfig
{
public:
	bool AreaGuard = false; // 按Ctrl+Alt巡航
	bool AutoGuard = false; // 移动巡航
	bool DefaultToGuard = false; // 自动起飞
	int GuardRange = 5;
	bool AutoFire = true;
	int MaxAmmo = 1;
	int MinAmmo = 0;
	int GuardRadius = 5;
	bool FindRangeAroundSelf = false;
	int ChaseRange = 30;
	bool Clockwise = false;
	bool Randomwise = false;


	virtual void Read(INIBufferReader* reader) override
	{
		Read(reader, "Fighter.");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		AreaGuard = reader->Get(title + "AreaGuard", AreaGuard);
		AutoGuard = reader->Get(title + "AutoGuard", AutoGuard);
		DefaultToGuard = reader->Get(title + "DefaultToGuard", DefaultToGuard);

		Enable = AreaGuard || AutoGuard || DefaultToGuard;

		GuardRange = reader->Get(title + "GuardRange", GuardRange);
		AutoFire = reader->Get(title + "AutoFire", AutoFire);
		MaxAmmo = reader->Get(title + "Ammo", MaxAmmo);
		MaxAmmo = reader->Get(title + "HoldAmmo", MaxAmmo);
		GuardRadius = reader->Get(title + "GuardRadius", GuardRadius);
		FindRangeAroundSelf = reader->Get(title + "FindRangeAroundSelf", FindRangeAroundSelf);
		ChaseRange = reader->Get(title + "ChaseRange", ChaseRange);
		Clockwise = reader->Get(title + "Clockwise", Clockwise);
		Randomwise = reader->Get(title + "Randomwise", Randomwise);

		Enable = AreaGuard || AutoGuard || DefaultToGuard;
	}
};
