#include "AutoFireAreaWeapon.h"

#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/ObjectType/AttachEffect.h>

AutoFireAreaWeaponData* AutoFireAreaWeapon::GetAutoAreaData()
{
	if (!_autoAreaData)
	{
		_autoAreaData = INI::GetConfig<AutoFireAreaWeaponData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _autoAreaData;
}

void AutoFireAreaWeapon::Setup()
{
	_autoAreaData = nullptr;
	if (!GetAutoAreaData()->Enable)
	{
		Disable();
	}
}

void AutoFireAreaWeapon::Awake()
{
	Setup();
}

void AutoFireAreaWeapon::ExtChanged()
{
	Setup();
}

void AutoFireAreaWeapon::OnPut(CoordStruct* pLocation, DirType dir)
{
	AutoFireAreaWeaponData* data = GetAutoAreaData();
	if (data->Enable)
	{
		_areaInitDelayTimer.Start(data->InitialDelay);
	}
}

void AutoFireAreaWeapon::OnUpdate()
{
	AutoFireAreaWeaponData* data = GetAutoAreaData();
	if (data->Enable && _areaInitDelayTimer.Expired() && _areaDelayTimer.Expired())
	{
		WeaponStruct* pWeaponStruct = pTechno->GetWeapon(data->WeaponIndex);
		WeaponTypeClass* pWeapon = pWeaponStruct ? pWeaponStruct->WeaponType : nullptr;
		if (pWeapon)
		{
			// 检查弹药
			int technoAmmo = pTechno->Ammo;
			int weaponAmmo = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon)->Ammo;
			if (technoAmmo >= 0)
			{
				int leftAmmo = technoAmmo - weaponAmmo;
				if (data->CheckAmmo || data->UseAmmo)
				{
					if (leftAmmo < 0)
					{
						return;
					}
				}
				if (data->UseAmmo)
				{
					pTechno->Ammo = leftAmmo;
					pTechno->StartReloading();
				}
			}
			int rof = pWeapon->ROF;
			AttachEffect* aeManager = nullptr;
			if (TryGetAEManager<TechnoExt>(pTechno, aeManager))
			{
				rof = (int)(rof * aeManager->CountAttachStatusMultiplier().ROFMultiplier);
			}
			_areaDelayTimer.Start(rof);
			AbstractClass* pTarget = pTechno;
			if (data->TargetToGround)
			{
				CoordStruct location = pTechno->GetCoords();
				if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
				{
					pTarget = pCell;
				}
			}
			if (pTarget)
			{
				SkipROF = true;
				pTechno->Fire_IgnoreType(pTarget, data->WeaponIndex);
				SkipROF = false;
			}

		}
	}
}
