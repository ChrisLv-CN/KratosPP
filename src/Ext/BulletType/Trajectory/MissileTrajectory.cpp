#include "MissileTrajectory.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

void MissileTrajectory::Awake()
{
	if (!IsMissile())
	{
		Disable();
	}
}

void MissileTrajectory::OnPut(CoordStruct* pCoord, DirType dirType)
{
	// 高抛导弹
	if (pBullet->WeaponType && pBullet->WeaponType->Lobber)
	{
		if (pBullet->Velocity.Z < 0)
		{
			pBullet->Velocity.Z *= -1;
		}
		pBullet->Velocity.Z += RulesClass::Instance->Gravity;
	}

	// 翻转发射方向
	if (trajectoryData->ReverseVelocity)
	{
		BulletVelocity velocity = pBullet->Velocity;
		pBullet->Velocity *= -1;
		if (!trajectoryData->ReverseVelocityZ)
		{
			pBullet->Velocity.Z = velocity.Z;
		}
	}

	// 晃动的出膛方向
	if (trajectoryData->ShakeVelocity != 0)
	{
		BulletVelocity velocity = pBullet->Velocity;
		double shakeX = Random::RandomDouble() * trajectoryData->ShakeVelocity;
		double shakeY = Random::RandomDouble() * trajectoryData->ShakeVelocity;
		double shakeZ = Random::RandomDouble();
		pBullet->Velocity.X *= shakeX;
		pBullet->Velocity.Y *= shakeY;
		pBullet->Velocity.Z *= shakeZ;
	}
}

// TODO 热诱弹
void MissileTrajectory::OnUpdate() {};

