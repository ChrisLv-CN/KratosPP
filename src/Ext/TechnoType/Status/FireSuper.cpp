#include "../TechnoStatus.h"

#include <Common/INI/INI.h>

#include <Ext/Common/FireSuperManager.h>

void TechnoStatus::OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx)
{
	if (FireSuper->IsActive())
	{
		FireSuperEntity data = FireSuper->Data.Data;
		if (pTechno->Veterancy.IsElite())
		{
			data = FireSuper->Data.EliteData;
		}
		if (data.Enable && (data.WeaponIndex < 0 || data.WeaponIndex == weaponIdx))
		{
			HouseClass* pHouse = pTechno->Owner;
			// 检查平民
			if (!FireSuper->Data.DeactiveWhenCivilian || !IsCivilian(pHouse))
			{
				CoordStruct targetPos = data.ToTarget ? pTarget->GetCoords() : pTechno->GetCoords();
				FireSuperManager::Launch(pHouse, targetPos, data);
			}
		}
	}
}

