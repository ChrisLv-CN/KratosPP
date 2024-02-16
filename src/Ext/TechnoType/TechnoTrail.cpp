#include "TechnoTrail.h"
#include "TechnoStatus.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Scripts.h>

void TechnoTrail::SetupTrails()
{
	_trails.clear();
	if (!TryGetTrails(pTechno->GetTechnoType()->ID, _trails))
	{
		Disable();
	}
}

void TechnoTrail::Awake()
{
	SetupTrails();
}

void TechnoTrail::ExtChanged()
{
	SetupTrails();
}

void TechnoTrail::OnPut(CoordStruct* pLocation, DirType dirType)
{
	if (_trails.empty())
	{
		SetupTrails();
	}
}

void TechnoTrail::OnRemove()
{
	_trails.clear();
}

void TechnoTrail::OnUpdate()
{
	if (!_setupFlag)
	{
		_setupFlag = true;
		SetupTrails();
	}
}

void TechnoTrail::OnUpdateEnd()
{
	if (!_trails.empty())
	{
		if (!IsDeadOrInvisibleOrCloaked(pTechno) && pTechno->GetHeight() >= 0)
		{
			TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno);
			for (Trail& trail : _trails)
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
			for (Trail& trail : _trails)
			{
				CoordStruct sourcePos = GetFLHAbsoluteCoords(pTechno, trail.FLH, trail.IsOnTurret);
				trail.UpdateLastLocation(sourcePos);
			}
		}
	}
}
