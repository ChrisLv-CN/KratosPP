#include "../BulletStatus.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

void BulletStatus::ShakeVelocity()
{
	if (!_missileShakeVelocityFlag)
	{
		_missileShakeVelocityFlag = true;
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
	}

void BulletStatus::InitState_Trajectory_Missile()
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


}

// TODO 热诱弹
void BulletStatus::OnUpdate_Trajectory_Decroy() {};

