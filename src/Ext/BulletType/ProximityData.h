#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

#include <Ext/Helper.h>

#include <YRPP.h>

class ProximityData : public INIConfig
{
public:
	// 碰撞引信
	bool Force = false;
	bool Blade = false;
	int Arm = 128;
	int ZOffset = Unsorted::LevelHeight;
	bool AffectsOwner = false;
	bool AffectsAllies = false;
	bool AffectsEnemies = true;
	bool AffectsClocked = true;

	// 穿透
	bool Penetration = false;
	std::string PenetrationWarhead;
	std::string PenetrationWeapon;
	int PenetrationTimes = -1;
	bool PenetrationBuildingOnce = false;

	virtual void Read(INIBufferReader* reader) override
	{
		Force = reader->Get("Proximity.Force", Force);
		Blade = reader->Get("Proximity.Blade", Blade);
		Arm = reader->Get("Proximity.Arm", Arm);
		ZOffset = reader->Get("Proximity.ZOffset", ZOffset);
		AffectsOwner = reader->Get("Proximity.AffectsOwner", AffectsOwner);
		AffectsAllies = reader->Get("Proximity.AffectsAllies", AffectsAllies);
		AffectsEnemies = reader->Get("Proximity.AffectsEnemies", AffectsEnemies);
		AffectsClocked = reader->Get("Proximity.AffectsClocked", AffectsClocked);

		Penetration = reader->Get("Proximity.Penetration", Penetration);
		PenetrationWarhead = reader->Get("Proximity.PenetrationWarhead", PenetrationWarhead);
		PenetrationWeapon = reader->Get("Proximity.PenetrationWeapon", PenetrationWeapon);
		PenetrationTimes = reader->Get("Proximity.PenetrationTimes", PenetrationTimes);
		PenetrationBuildingOnce = reader->Get("Proximity.PenetrationBuildingOnce", PenetrationBuildingOnce);
	}

};


/**
 *@brief 碰撞引信的状态
 *
 */
struct Proximity
{
public:
	CellClass* pCheckedCell = nullptr;
	std::vector<BuildingClass*> BuildingMarks{};

	bool count = true;
	int times = 1;

	bool safe = false;
	CDTimerClass safeTimer{};

	Proximity() {}

	Proximity(int safeDelay, int times)
	{
		this->safe = safeDelay > 0;
		if (safe)
		{
			this->safeTimer.Start(safeDelay);
		}
		this->count = times > 0;
		this->times = times;
	}

	bool IsSafe()
	{
		if (safe)
		{
			safe = safeTimer.InProgress();
		}
		return safe;
	}

	void ThroughOnce()
	{
		if (count)
		{
			times--;
		}
	}

	bool TimesDone()
	{
		return count && times <= 0;
	}

	bool CheckAndMarkBuilding(BuildingClass* pBuilding)
	{
		auto it = std::find(BuildingMarks.begin(), BuildingMarks.end(), pBuilding);
		bool find = it != BuildingMarks.end();
		if (!find)
		{
			BuildingMarks.emplace_back(pBuilding);
		}
		return find;
	}
};

