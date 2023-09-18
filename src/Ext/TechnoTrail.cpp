﻿#include "TechnoTrail.h"
#include "TechnoStatus.h"

#include <Ext/Helper.h>

void TechnoTrail::SetupTrails()
{
	trails.clear();
	TryGetTrails(pTechno->GetTechnoType()->ID, trails);
}

bool TechnoTrail::OnAwake()
{
	if (pTechno->AbstractFlags & AbstractFlags::Foot)
	{
		return true;
	}
	return false;
}

void TechnoTrail::OnTransform(TypeChangeEventArgs* args)
{
	TechnoClass* pTarget = args->pTechno;
	if (pTarget && pTarget == pTechno)
	{
		SetupTrails();
	}
}

void TechnoTrail::OnPut(CoordStruct* pLocation, DirType dirType)
{
	if (trails.empty())
	{
		SetupTrails();
	}
}

void TechnoTrail::OnRemove()
{
	trails.clear();
}

void TechnoTrail::OnUpdateEnd()
{
	if (!trails.empty())
	{
		if (!IsDeadOrInvisibleOrCloaked(pTechno) && pTechno->GetHeight() >= 0)
		{
			TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno);
			for (Trail& trail : trails)
			{
				// 检查动画尾巴
				if (trail.GetMode() == TrailMode::ANIM)
				{
					switch (status->drivingState)
					{
					case DrivingState::Start:
					case DrivingState::Stop:
						trail.SetDrivingState(status->drivingState);
						break;
					}
				}
				CoordStruct sourcePos = GetFLHAbsoluteCoords(pTechno, trail.FLH, trail.IsOnTurret);
				trail.DrawTrail(sourcePos, pTechno->Owner, pTechno);
			}
		}
		else
		{
			// 更新坐标
			for (Trail& trail : trails)
			{
				CoordStruct sourcePos = GetFLHAbsoluteCoords(pTechno, trail.FLH, trail.IsOnTurret);
				trail.UpdateLastLocation(sourcePos);
			}
		}
	}
}