#include "OverrideWeaponState.h"

bool OverrideWeaponState::CanOverride(bool isElite, bool isDeathWeapon, std::string& weaponID)
{

	if (Data.Enable && (isDeathWeapon || _weaponIndex >= 0))
	{
		OverrideWeaponEntity data = isElite ? Data.EliteData : Data.Data;
		if (data.Enable)
		{
			weaponID = data.Types[0];

			int overrideIndex = data.Index;
			double chance = data.Chance;
			if (data.RandomType)
			{
				// 计算权重
				int maxValue = 0;
				std::map<Point2D, int> targetPad = MakeTargetPad(data.Weights, data.Types.size(), maxValue);
				int index = Hit(targetPad, maxValue);
				weaponID = data.Types[index];
			}
			if (IsNotNone(weaponID)
				&& (overrideIndex < 0 || isDeathWeapon || overrideIndex == _weaponIndex))
			{
				return chance >= 1 || Bingo(chance);
			}
		}
	}
	return false;
}

bool OverrideWeaponState::TryGetOverrideWeapon(bool isElite, bool isDeadWeapon, WeaponTypeClass*& pOverrideWeapon)
{
	pOverrideWeapon = nullptr;
	std::string weaponId{ "" };
	if (IsAlive() && (!Data.UseToDeathWeapon || isDeadWeapon)
		&& CanOverride(isElite, isDeadWeapon, weaponId)
		&& IsNotNone(weaponId))
	{
		pOverrideWeapon = WeaponTypeClass::Find(weaponId.c_str());
		return pOverrideWeapon != nullptr;
	}
	return false;
}

void OverrideWeaponState::OnFire(AbstractClass* pTarget, int weaponIndex)
{
	_weaponIndex = weaponIndex;
}

