#include "..\BulletStatus.h"
#include <Ext/Helper.h>
#include <Ext/WeaponType/ProximityRangeData.h>

ProximityData* BulletStatus::GetProximityData()
{
	if (!_proximityData)
	{
		_proximityData = INI::GetConfig<ProximityData>(INI::Rules, pBullet->GetType()->ID)->Data;
	}
	return _proximityData;
}

void BulletStatus::ActiveProximity()
{
	this->proximity = { pBullet->Type->CourseLockDuration, GetProximityData()->PenetrationTimes };
	this->_activeProximity = true;
}

void BulletStatus::InitState_Proximity() {
	// 设置提前引爆抛射体
	WeaponTypeClass* pWeapon = pBullet->WeaponType;
	if (pWeapon)
	{
		const char* weaponId = pWeapon->ID;
		ProximityRangeData* tempData = INI::GetConfig<ProximityRangeData>(INI::Rules, weaponId)->Data;
		int range = tempData->GetRange();
		if (range > 0)
		{
			this->_proximityRange = range;
			AbstractClass* pTarget = pBullet->Target;
			if (pTarget && pTarget->WhatAmI() == AbstractType::Building)
			{
				BuildingTypeClass* pTargetBuildingType = ((BuildingClass*)pTarget)->Type;
				int distOffset = (pTargetBuildingType->GetFoundationWidth() + pTargetBuildingType->GetFoundationHeight(false)) << 6;
				this->_proximityRange += distOffset;
			}
		}
	}
	// 设置碰触引擎
	if (GetProximityData()->Force)
	{
		ActiveProximity();
	}
};

void BulletStatus::OnUpdateEnd_Proximity(CoordStruct& sourcePos) {};
