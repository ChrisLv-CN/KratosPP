﻿#include "BulletTrail.h"
#include "BulletStatus.h"

BULLET_SCRIPT_CPP(BulletTrail);

void BulletTrail::SetupTrails()
{
	_trails.clear();
	if (!TryGetTrails(pBullet->Type->ID, _trails))
	{
		_gameObject->RemoveComponent(this);
	}
}

void BulletTrail::OnUpdate()
{
	if (!_setupFlag)
	{
		_setupFlag = true;
		SetupTrails();
	}
}

void BulletTrail::OnUpdateEnd()
{
	if (!IsDeadOrInvisible(pBullet) && pBullet->GetHeight() >= 0)
	{
		if (!_trails.empty())
		{
			TechnoClass* pSource = nullptr;
			HouseClass* pHouse = nullptr;
			if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
			{
				pSource = status->pSource;
				pHouse = status->pSourceHouse;
			}
			CoordStruct location = pBullet->GetCoords();
			DirStruct bulletFacing = Facing(pBullet, location);
			for(Trail& trail : _trails)
			{
				CoordStruct sourcePos = GetFLHAbsoluteCoords(location, trail.FLH, bulletFacing);
				trail.DrawTrail(sourcePos, pHouse, pSource);
			}
		}
	}
}
