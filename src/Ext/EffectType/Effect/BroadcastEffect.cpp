#include "BroadcastEffect.h"

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

void BroadcastEffect::FindAndAttach(BroadcastEntity data, HouseClass* pHouse)
{
	CoordStruct location = pObject->GetCoords();
	ObjectClass* pSource = AE->pSource;
	HouseClass* pSourceHouse = AE->pSourceHouse;
	if (AEData.ReceiverOwn || !pSource || !pSourceHouse)
	{
		pSource = pObject;
		pSourceHouse = pHouse;
	}
	// 搜索单位
	if (Data->AffectTechno)
	{
		FindTechnoOnMark([&](TechnoClass* pTarget, AttachEffect* aeManager)
			{
				// 赋予AE
				aeManager->Attach(data.Types, data.AttachChances, false, pSource, pSourceHouse);
				return false;
			}, location, data.RangeMax, data.RangeMin, data.FullAirspace, pSourceHouse, *Data, pObject);
	}
	// 搜索抛射体
	if (Data->AffectBullet)
	{
		FindBulletOnMark([&](BulletClass* pTarget, AttachEffect* aeManager)
			{
				// 赋予AE
				aeManager->Attach(data.Types, data.AttachChances, false, pSource, pSourceHouse);
				return false;
			}, location, data.RangeMax, data.RangeMin, data.FullAirspace, pSourceHouse, *Data, pObject);
	}
}

void BroadcastEffect::OnUpdate()
{
	if (!AE->OwnerIsDead())
	{
		if (Data->Powered && AE->AEManager->PowerOff)
		{
			// 需要电力，但是没电
			return;
		}
		BroadcastEntity data = Data->Data;
		HouseClass* pHouse = nullptr;
		if (pTechno)
		{
			pHouse = pTechno->Owner;
			if (pTechno->Veterancy.IsElite())
			{
				data = Data->EliteData;
			}
		}
		else if (pBullet)
		{
			pHouse = GetSourceHouse(pBullet);
		}
		else
		{
			return;
		}

		// 检查平民
		if (Data->DeactiveWhenCivilian && IsCivilian(pHouse))
		{
			return;
		}
		if (data.Enable)
		{
			if (_delayTimer.Expired())
			{
				// 检查次数
				if (Data->TriggeredTimes > 0 && ++_count >= Data->TriggeredTimes)
				{
					// 结束效果器
					Deactivate();
					AE->TimeToDie();
				}
				_delayTimer.Start(data.Rate);
				FindAndAttach(data, pHouse);
			}
		}
	}
}
