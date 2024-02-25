#include "StraightTrajectory.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

#include <Extension/WeaponTypeExt.h>

#include "../BulletStatus.h"

void StraightTrajectory::ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos)
{
	CoordStruct sourcePos = pBullet->GetCoords();
	pBullet->SourceCoords = sourcePos;
	Setup();
}

void StraightTrajectory::ResetStraightMissileVelocity()
{
	_straightBullet.ResetVelocity(pBullet);
}

void StraightTrajectory::Setup()
{
	// 直线弹道
	CoordStruct sourcePos = pBullet->SourceCoords;
	CoordStruct targetPos = pBullet->TargetCoords;

	TrajectoryData* trajectoryData = INI::GetConfig<TrajectoryData>(INI::Rules, pBullet->GetType()->ID)->Data;
	// 绝对直线，重设目标坐标
	if (trajectoryData->AbsolutelyStraight && pBullet->Owner)
	{
		double distance = targetPos.DistanceFrom(sourcePos);
		DirStruct facing = pBullet->Owner->GetRealFacing().Current();
		targetPos = GetFLHAbsoluteCoords(sourcePos, CoordStruct{ (int)distance, 0, 0 }, facing);
		pBullet->TargetCoords = targetPos;

		// BulletEffectHelper.BlueLine(pBullet->SourceCoords, pBullet->TargetCoords, 1, 90);
	}
	// 重设速度
	BulletVelocity velocity = pBullet->Velocity;
	bool reset = true;
	if (pBullet->WeaponType)
	{
		WeaponTypeExt::TypeData* weaponTypeData = WeaponTypeExt::GetData<WeaponTypeExt::TypeData>(pBullet->WeaponType);
		if (weaponTypeData->RadialFire)
		{
			BulletVelocity sourceV = ToVelocity(sourcePos);
			BulletVelocity targetV = sourceV + velocity;
			CoordStruct forward = GetForwardCoords(sourceV, targetV, pBullet->Speed);
			CoordStruct offset = forward - sourcePos;
			velocity = ToVelocity(offset);
			if (pBullet->Type->Level)
			{
				velocity.Z = 0;
			}
			reset = false;
		}
	}
	if (reset)
	{
		velocity = RecalculateBulletVelocity(pBullet, sourcePos, targetPos);
	}
	// 记录下导弹的速度，之后在Update时用该速度重写抛射体
	_straightBullet = { sourcePos, targetPos, velocity };

	// 设置触碰引信
	if (pBullet->Type->Proximity)
	{
		dynamic_cast<BulletStatus*>(_parent)->ActiveProximity();
	}
}

void StraightTrajectory::Awake()
{
	if (!IsRocket())
	{
		Disable();
	}
}

void StraightTrajectory::OnPut(CoordStruct* pCoord, DirType dirType)
{
	Setup();
}

void StraightTrajectory::OnUpdate()
{
	if (!dynamic_cast<BulletStatus*>(_parent)->CaptureByBlackHole)
	{
		// 强制修正速度
		pBullet->Velocity = _straightBullet.Velocity;
	}

	// 看不懂西木的做法，自己来
	AbstractClass* pTarget = pBullet->Target;
	if (pTarget && !_resetTargetFlag)
	{
		CoordStruct targetCoord = pBullet->TargetCoords;
		CoordStruct targetPos = pTarget->GetCoords();
		if (pTarget->IsInAir())
		{
			// 目标在空中，目标当前的距离和目标位置相差一个格子，则取消目标
			CellClass* pCell = MapClass::Instance->TryGetCellAt(targetPos);
			CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(targetCoord);
			if (pCell != nullptr && pTargetCell != nullptr)
			{
				if (pCell != pTargetCell)
				{
					pBullet->SetTarget(nullptr);
					_resetTargetFlag = true;
				}
			}
			else if (targetCoord.DistanceFrom(targetPos) >= 128)
			{
				pBullet->SetTarget(nullptr);
				_resetTargetFlag = true;
			}
		}
		else
		{
			// 目标离开所在的格子，就设定目标位置的格子为目标
			CellClass* pCell = MapClass::Instance->TryGetCellAt(targetPos);
			CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(targetCoord);
			if (pCell != nullptr && pTargetCell != nullptr)
			{
				if (pCell != pTargetCell)
				{
					pBullet->SetTarget(pTargetCell);
					_resetTargetFlag = true;
				}
			}
		}
	}
}
