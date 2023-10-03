#include "../TechnoStatus.h"

#include <Extension/WeaponTypeExt.h>

AutoFireAreaWeaponData* TechnoStatus::GetAutoAreaData()
{
	if (!_autoAreaData)
	{
		_autoAreaData = INI::GetConfig<AutoFireAreaWeaponData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _autoAreaData;
}

void TechnoStatus::OnPut_AutoArea(CoordStruct* pLocation, DirType dir)
{
	AutoFireAreaWeaponData* data = GetAutoAreaData();
	if (data->Enable)
	{
		_areaInitDelayTimer.Start(data->InitialDelay);
	}
}

void TechnoStatus::OnUpdate_AutoArea()
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
			/* TODO AE
			if (pTechno.TryGetAEManager(out AttachEffectScript aeManager))
			{
				rof = (int)(rof * aeManager.CountAttachStatusMultiplier().ROFMultiplier);
			}*/
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
