#include "..\BulletStatus.h"
#include <Ext/Helper.h>

void BulletStatus::OnUpdate_Trajectory_Arcing()
{
	ResetArcingVelocity();
}

void BulletStatus::ResetArcingVelocity(float speedMultiple, bool force)
{
	if (force || !_arcingTrajectoryInitFlag && trajectoryData->AdvancedBallistics)
	{
		_arcingTrajectoryInitFlag = true;
		CoordStruct sourcePos = pBullet->GetCoords();
		CoordStruct targetPos = pBullet->TargetCoords;

		if (trajectoryData->ArcingFixedSpeed > 0)
		{
			pBullet->Speed = trajectoryData->ArcingFixedSpeed;
		}
		else
		{
			pBullet->Speed += pBullet->Type->Acceleration;
		}

		int speed = (int)(pBullet->Speed * speedMultiple);
		int gravity = RulesClass::Instance->Gravity;
		if (trajectoryData->Gravity > 0)
		{
			gravity = trajectoryData->Gravity;
		}
		bool lobber = pBullet->WeaponType ? pBullet->WeaponType->Lobber : false;
		bool inaccurate = trajectoryData->Inaccurate;
		float min = trajectoryData->BallisticScatterMin;
		float max = trajectoryData->BallisticScatterMax;

		double straightDistance = 0;
		double realSpeed = 0;
		CellClass* pTargetCell = nullptr;
		BulletVelocity velocity = GetBulletArcingVelocity(sourcePos, targetPos,
			speed, gravity, lobber, inaccurate, min, max,
			ToCoordStruct(pBullet->Velocity).Z, straightDistance, realSpeed, pTargetCell);
		pBullet->Speed = (int)realSpeed;
		pBullet->Velocity = velocity;
		pBullet->TargetCoords = targetPos;
		if (inaccurate && pTargetCell)
		{
			pBullet->Target = pTargetCell;
		}

	}
}

#pragma region Bounce Arcing
void BulletStatus::InitState_Bounce() {};

void BulletStatus::OnUpdate_Trajectory_Bounce() {};

bool BulletStatus::OnDetonate_Bounce(CoordStruct* pCoords) { return false; };
#pragma endregion
