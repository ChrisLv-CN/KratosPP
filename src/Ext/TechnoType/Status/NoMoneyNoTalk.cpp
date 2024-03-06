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
		if (needMoney == 0 && NoMoneyNoTalk->IsAlive())
		{
			needMoney = NoMoneyNoTalk->Data.Money;
		}
		if (needMoney != 0 && pHouse)
		{
			int money = pHouse->Available_Money();
			if (needMoney > 0 ? money < needMoney : money > abs(needMoney))
			{
				return true;
			}
		}
	}
	return false;
}
