﻿#include "../TechnoStatus.h"

#include <Ext/FireSuperManager.h>
#include <Ext/AttachFire.h>

void TechnoStatus::InitState_FireSuper()
{
	FireSuperData* data = INI::GetConfig<FireSuperData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	if (data->Enable)
	{
		FireSuperState.Enable(*data);
	}
}

void TechnoStatus::OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx)
{
	if (FireSuperState.IsActive())
	{
		FireSuperEntity data = FireSuperState.Data.Data;
		if (pTechno->Veterancy.IsElite())
		{
			data = FireSuperState.Data.EliteData;
		}
		if (data.Enable && (data.WeaponIndex < 0 || data.WeaponIndex == weaponIdx))
		{
			HouseClass* pHouse = pTechno->Owner;
			// 检查平民
			if (!FireSuperState.Data.DeactiveWhenCivilian || !IsCivilian(pHouse))
			{
				CoordStruct targetPos = data.ToTarget ? pTarget->GetCoords() : pTechno->GetCoords();
				FireSuperManager::Launch(pHouse, targetPos, data);
			}
		}
	}
}

