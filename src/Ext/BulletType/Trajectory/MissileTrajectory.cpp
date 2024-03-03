#include "MissileTrajectory.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/TechnoType/DecoyMissile.h>


void MissileTrajectory::CheckTargetHasDecoy()
{
	if (!_targetHasDecoyFlag)
	{
		_targetHasDecoyFlag = true;

		AbstractClass* pTarget = pBullet->Target;
		TechnoClass* pTargetTechno = nullptr;
		DecoyMissile* status = nullptr;
		if (CastToTechno(pTarget, pTargetTechno) && TryGetScript<TechnoExt>(pTargetTechno, status))
		{
			BulletExt::TargetHasDecoyBullets.push_back(pBullet);
		}
	}
}

BulletStatus* MissileTrajectory::GetBulletStatus()
{
	if (!_status)
	{
		_status = GetStatus<BulletExt, BulletStatus>(pBullet);
	}
	return _status;
}

void MissileTrajectory::Awake()
{
	if (!IsMissile())
	{
		Disable();
	}
}

void MissileTrajectory::Destroy()
{
	auto it = std::find(BulletExt::TargetHasDecoyBullets.begin(), BulletExt::TargetHasDecoyBullets.end(), pBullet);
	if (it != BulletExt::TargetHasDecoyBullets.end())
	{
		BulletExt::TargetHasDecoyBullets.erase(it);
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

	// 是否是对飞行器攻击
	CheckTargetHasDecoy();
}

// 热诱弹
void MissileTrajectory::OnUpdate()
{
	// 是否是对飞行器攻击
	CheckTargetHasDecoy();
	if (IsDecoy)
	{
		// 检查存活时间
		if (LifeTimer.Expired())
		{
			if (BulletStatus* status = GetBulletStatus())
			{
				status->TakeDamage();
			}
		}
		else
		{
			// 执行热诱弹轨迹变化
			// Check distance to Change speed and target point
			int speed = pBullet->Speed - 5;
			pBullet->Speed = speed < 10 ? 10 : speed;
			if (speed > 10 && LaunchPos.DistanceFrom(pBullet->GetCoords()) <= pBullet->WeaponType->Range)
			{
				pBullet->Location += CoordStruct{ 0, 0, 64 };
			}
		}
	}
};

