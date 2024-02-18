#include "ArcingTrajectory.h"

#include <Ext/Helper/Physics.h>
#include <Ext/Helper/Weapon.h>

void ArcingTrajectory::ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos)
{
	CoordStruct sourcePos = pBullet->GetCoords();
	pBullet->SourceCoords = sourcePos;
	ResetArcingVelocity(1.0f);
}

void ArcingTrajectory::ResetArcingVelocity(float speedMultiple)
{
	if (trajectoryData->AdvancedBallistics)
	{
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

void ArcingTrajectory::Awake()
{
	if (!IsArcing())
	{
		Disable();
	}
}


void ArcingTrajectory::OnUpdate()
{
	if (!_initFlag)
	{
		_initFlag = true;
		ResetArcingVelocity();
	}
	// 有弹性，撞悬崖会反弹
	if (CanBounce)
	{
		CoordStruct location = pBullet->GetCoords();
		if (pBullet->GetHeight() > 0)
		{
			// 检查撞悬崖反弹
			CoordStruct nextPos = location + ToCoordStruct(pBullet->Velocity);
			CoordStruct nextCellPos = CoordStruct::Empty;
			bool onBridge = false;
			switch (CanMoveTo(location, nextPos, true, nextCellPos, onBridge))
			{
			case PassError::HITWALL:
				// 重设预定目标位置
				CoordStruct v = pBullet->TargetCoords - location;
				v.X *= -1;
				v.Y *= -1;
				CoordStruct newTargetPos = location + v;
				// 反弹
				pBullet->Velocity.X *= -1;
				pBullet->Velocity.Y *= -1;
				pBullet->SourceCoords = location;
				pBullet->TargetCoords = newTargetPos;
				break;
			}
		}
	}
}
