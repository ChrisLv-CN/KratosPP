#include "BlackHoleState.h"

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Scripts.h>

#include <Extension/WarheadTypeExt.h>

#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/BulletType/BulletStatus.h>

BlackHoleEntity BlackHoleState::GetDataEntity()
{
	BlackHoleEntity data = Data.Data;
	if (_isElite)
	{
		data = Data.EliteData;
	}
	return data;
}

void BlackHoleState::Reload()
{
	BlackHoleEntity data = GetDataEntity();
	_delay = data.Rate;
	if (_delay > 0)
	{
		_delayTimer.Start(_delay);
	}
	_count++;
}

bool BlackHoleState::IsReady()
{
	return !IsDone() && Timeup();
}

bool BlackHoleState::Timeup()
{
	return _delay <= 0 || _delayTimer.Expired();
}

bool BlackHoleState::IsDone()
{
	return Data.TriggeredTimes > 0 && _count >= Data.TriggeredTimes;
}

bool BlackHoleState::IsOutOfRange(CoordStruct targetPos)
{
	BlackHoleEntity data = GetDataEntity();
	CoordStruct sourcePos = pObject->GetCoords();
	double dist = sourcePos.DistanceFrom(targetPos);
	return isnan(dist) || isinf(dist) || !data.Enable || data.Range == 0 || (data.Range > 0 && dist > data.Range * Unsorted::LeptonsPerCell);
}

void BlackHoleState::StartCapture()
{
	HouseClass* pHouse = pAEHouse;
	if (!pHouse)
	{
		if (pTechno)
		{
			pHouse = pTechno->Owner;
		}
		else if (pBullet)
		{
			pHouse = GetHouse(pBullet);
		}
	}
	BlackHoleEntity data = GetDataEntity();
	if (!Data.DontScan && data.Enable)
	{
		Reload();
		// 检查平民
		if (Data.DeactiveWhenCivilian && IsCivilian(pHouse))
		{
			return;
		}
		CoordStruct location = pObject->GetCoords();
		if (Data.AffectTechno)
		{
			FindTechnoOnMark([&](TechnoClass* pTarget, AttachEffect* aem) {
				if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTarget))
				{
					status->BlackHoleCapture(pObject, Data);
				}
				}, location, data.Range, 0, data.FullAirspace, pHouse, Data, pObject);
		}
		if (Data.AffectBullet)
		{
			FindBulletOnMark([&](BulletClass* pTarget, AttachEffect* aem) {
				if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pTarget))
				{
					status->BlackHoleCapture(pObject, Data);
				}
				}, location, data.Range, 0, data.FullAirspace, pHouse, Data, pObject);
		}
	}
}

void BlackHoleState::OnStart()
{
	_count = 0;
	_delay = 0;
	_delayTimer.Stop();
	_isElite = false;
}

void BlackHoleState::OnUpdate()
{
	if (IsDone())
	{
		End();
		return;
	}
	if (pTechno)
	{
		_isElite = pTechno->Veterancy.IsElite();
	}
	if (!IsDeadOrInvisible(pObject) && IsReady())
	{
		StartCapture();
	}
}
