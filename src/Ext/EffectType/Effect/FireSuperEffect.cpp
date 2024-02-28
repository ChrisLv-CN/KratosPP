#include "FireSuperEffect.h"

#include <Ext/Common/FireSuperManager.h>

void FireSuperEffect::OnFire(AbstractClass* pTarget, int weaponIdx)
{
	if (_delayTimer.Expired())
	{
		FireSuperEntity data = Data->Data;
		if (pTechno->Veterancy.IsElite())
		{
			data = Data->EliteData;
		}
		if (data.Enable && (data.WeaponIndex < 0 || data.WeaponIndex == weaponIdx))
		{
			HouseClass* pHouse = pTechno->Owner;
			// 检查平民
			if (!Data->DeactiveWhenCivilian || !IsCivilian(pHouse))
			{
				CoordStruct targetPos = data.ToTarget ? pTarget->GetCoords() : pTechno->GetCoords();
				FireSuperManager::Launch(pHouse, targetPos, data);

				// 检查触发次数
				if (Data->TriggeredTimes > 0 && ++_count >= Data->TriggeredTimes)
				{
					Deactivate();
					AE->TimeToDie();
				}
				if (data.Reload > 0)
				{
					_delayTimer.Start(data.Reload);
				}
			}
		}
	}
}
