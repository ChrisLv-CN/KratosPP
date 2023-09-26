#include "../TechnoStatus.h"

#include <Ext/FireSuperManager.h>

void TechnoStatus::OnFire_FireSuper(AbstractClass* pTarget, int weaponIdx)
{
	/*
	FireSuperEntity fireData{};
	fireData.Supers.push_back("UnitDeliverySpecial");
	fireData.Delay = 150;
	fireData.LaunchCount = 10;
	CoordStruct targetPos = pTarget->GetCoords();
	FireSuperManager::Order(pTechno->Owner, targetPos, fireData);*/
	if (pTarget->AbstractFlags & AbstractFlags::Techno)
	{
		//TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(dynamic_cast<TechnoClass*>(pTarget));
		//status->PaintballState.RGBIsPower();
	}
}

