#include "../TechnoStatus.h"

#include <Ext/FireSuperManager.h>
#include <Ext/AttachFire.h>

void TechnoStatus::OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx)
{
	/*
	FireSuperEntity fireData{};
	fireData.Supers.push_back("UnitDeliverySpecial");
	fireData.Delay = 150;
	fireData.LaunchCount = 10;
	CoordStruct targetPos = pTarget->GetCoords();
	FireSuperManager::Order(pTechno->Owner, targetPos, fireData);*/
	AttachFire<TechnoClass, TechnoExt>* attachFire = _gameObject->GetComponent<AttachFire<TechnoClass, TechnoExt>>();
	if (attachFire)
	{
		attachFire->FireCustomWeapon(pTechno, pTarget, pTechno->Owner, "AngelMissile", { -150, 0, 0 });
	}

}

