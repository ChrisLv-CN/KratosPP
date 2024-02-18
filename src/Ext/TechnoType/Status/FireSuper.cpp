#include "../TechnoStatus.h"

#include <Common/INI/INI.h>

#include <Ext/Common/FireSuperManager.h>

void TechnoStatus::OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx)
{
	if (FireSuperState->IsActive())
	{
		FireSuperEntity data = FireSuperState->Data.Data;
		if (pTechno->Veterancy.IsElite())
		{
			data = FireSuperState->Data.EliteData;
		}
		if (data.Enable && (data.WeaponIndex < 0 || data.WeaponIndex == weaponIdx))
		{
			HouseClass* pHouse = pTechno->Owner;
			// 检查平民
			if (!FireSuperState->Data.DeactiveWhenCivilian || !IsCivilian(pHouse))
			{
				CoordStruct targetPos = data.ToTarget ? pTarget->GetCoords() : pTechno->GetCoords();
				FireSuperManager::Launch(pHouse, targetPos, data);
			}
		}
	}
}

