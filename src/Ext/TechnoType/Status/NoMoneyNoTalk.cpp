#include "../TechnoStatus.h"

#include <Extension/WeaponTypeExt.h>

#include <Ext/Helper/Scripts.h>

bool TechnoStatus::CanFire_NoMoneyNoTalk(AbstractClass* pTarget, WeaponTypeClass* pWeapon)
{
	HouseClass* pHouse = pTechno->Owner;
	WeaponTypeExt::TypeData* weaponData = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon);
	if (weaponData && !weaponData->DontNeedMoney)
	{
		int needMoney = weaponData->NoMoneyNoTalk;
		if (needMoney == 0 && NoMoneyNoTalk->IsActive())
		{
			needMoney = NoMoneyNoTalk->Data.Money;
		}
		if (needMoney != 0 && pHouse)
		{
			if (needMoney > 0)
			{
				return pHouse->Available_Money() < needMoney;
			}
			else if (needMoney < 0)
			{
				return pHouse->Available_Money() > abs(needMoney);
			}
		}
	}
	return false;
}
