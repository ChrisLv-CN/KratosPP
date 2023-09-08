#include "..\BulletStatus.h"
#include <Ext/Helper.h>

void BulletStatus::OnUpdate_Trajectory_Arcing()
{
	ResetArcingVelocity();
}

void BulletStatus::ResetArcingVelocity(float speedMultiple, bool force)
{
	if (force || !_arcingTrajectoryInitFlag && GetTrajectoryData()->AdvancedBallistics)
	{
		_arcingTrajectoryInitFlag = true;
		CoordStruct sourcePos = _owner->GetCoords();
		CoordStruct targetPos = _owner->TargetCoords;

		if (GetTrajectoryData()->ArcingFixedSpeed > 0)
		{
			_owner->Speed = GetTrajectoryData()->ArcingFixedSpeed;
		}
		else
		{
			_owner->Speed += _owner->Type->Acceleration;
		}

		int speed = (int)(_owner->Speed * speedMultiple);
		int gravity = RulesClass::Instance->Gravity;
		if (GetTrajectoryData()->Gravity > 0)
		{
			gravity = GetTrajectoryData()->Gravity;
		}
		bool lobber = _owner->WeaponType ? _owner->WeaponType->Lobber : false;
		bool inaccurate = GetTrajectoryData()->Inaccurate;
		float min = GetTrajectoryData()->BallisticScatterMin;
		float max = GetTrajectoryData()->BallisticScatterMax;

		double straightDistance = 0;
		double realSpeed = 0;
		CellClass* pTargetCell = nullptr;
		BulletVelocity velocity = GetBulletArcingVelocity(sourcePos, targetPos,
			speed, gravity, lobber, inaccurate, min, max,
			ToCoordStruct(_owner->Velocity).Z, straightDistance, realSpeed, pTargetCell);
		_owner->Speed = (int)realSpeed;
		_owner->Velocity = velocity;
		_owner->TargetCoords = targetPos;
		if (inaccurate && pTargetCell)
		{
			_owner->Target = pTargetCell;
		}

	}
}
