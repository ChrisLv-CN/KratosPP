﻿#include "BulletStatus.h"

#include <Extension/WarheadTypeExt.h>
#include <Ext/Helper.h>
#include <Ext/TechnoStatus.h>

BULLET_SCRIPT_CPP(BulletStatus);

BulletType BulletStatus::GetBulletType()
{
	if (_bulletType == BulletType::UNKNOWN)
	{
		_bulletType = WhatAmI(pBullet);
		if (_bulletType != BulletType::ROCKET && trajectoryData->IsStraight())
		{
			_bulletType = BulletType::ROCKET;
		}
	}
	return _bulletType;
}

bool BulletStatus::IsArcing()
{
	return GetBulletType() == BulletType::ARCING;
}

bool BulletStatus::IsMissile()
{
	return GetBulletType() == BulletType::MISSILE;
}

bool BulletStatus::IsRocket()
{
	return GetBulletType() == BulletType::ROCKET;
}

bool BulletStatus::IsBomb()
{
	return GetBulletType() == BulletType::BOMB;
}

void BulletStatus::Awake()
{
	pSource = pBullet->Owner;
	if (pSource)
	{
		pSourceHouse = pSource->Owner;
	}
	// 读取生命值
	int health = pBullet->Health;
	// 抛射体武器伤害为复述或者零时需要处理
	if (health < 0)
	{
		health = -health;
	}
	else if (health == 0)
	{
		health = 1; // 武器伤害为0，如[NukeCarrier]
	}

	INIBufferReader* reader = INI::GetSection(INI::Rules, pBullet->GetType()->ID);
	// 初始化生命
	this->life.Health = health;
	this->life.Read(reader);
	// 初始化伤害
	this->damage.Damage = health;
	if (TechnoStatus* sourceStatue = GetStatus<TechnoExt, TechnoStatus>(pSource))
	{
		if (sourceStatue->AntiBulletState.IsActive())
		{
			damage.Eliminate = sourceStatue->AntiBulletState.Data.OneShotOneKill;
			damage.Harmless = sourceStatue->AntiBulletState.Data.Harmless;
		}
	}

	// 初始化地面碰撞属性
	switch (trajectoryData->SubjectToGround)
	{
	case SubjectToGroundType::YES:
		this->SubjectToGround = true;
		break;
	case SubjectToGroundType::NO:
		this->SubjectToGround = false;
		break;
	default:
		this->SubjectToGround = !IsArcing() && !IsRocket() && !trajectoryData->IsStraight();
		break;
	}
}

void BulletStatus::Destroy()
{
	auto it = std::find(BulletExt::TargetAircraftBullets.begin(), BulletExt::TargetAircraftBullets.end(), pBullet);
	if (it != BulletExt::TargetAircraftBullets.end())
	{
		BulletExt::TargetAircraftBullets.erase(it);
	}
}

void BulletStatus::TakeDamage(int damage, bool eliminate, bool harmless, bool checkInterceptable)
{
	if (!checkInterceptable || life.Interceptable)
	{
		if (eliminate)
		{
			life.Detonate(harmless);
		}
		else
		{
			life.TakeDamage(damage, harmless);
		}
	}
}

void BulletStatus::TakeDamage(BulletDamage damageData, bool checkInterceptable)
{
	TakeDamage(damageData.Damage, damageData.Eliminate, damageData.Harmless, checkInterceptable);
}

void BulletStatus::ResetTarget(AbstractClass* pNewTarget, CoordStruct targetPos)
{
	pBullet->SetTarget(pNewTarget);
	if (targetPos == CoordStruct::Empty && pNewTarget)
	{
		targetPos = pNewTarget->GetCoords();
	}
	pBullet->TargetCoords = targetPos;
	// 重设弹道
	if (IsArcing())
	{
		CoordStruct sourcePos = pBullet->GetCoords();
		pBullet->SourceCoords = sourcePos;
		ResetArcingVelocity(1.0f, true);
	}
	else if (IsRocket())
	{
		CoordStruct sourcePos = pBullet->GetCoords();
		pBullet->SourceCoords = sourcePos;
		InitState_Trajectory_Straight();
	}
}

void BulletStatus::OnPut(CoordStruct* pLocation, DirType dir)
{
	if (!_initFlag)
	{
		_initFlag = true;
		InitState_BlackHole();
		InitState_Bounce();
		InitState_DestroySelf();
		InitState_GiftBox();
		InitState_Paintball();
		InitState_Proximity();
		// 弹道初始化
		if (IsMissile())
		{
			InitState_ECM();
			InitState_Trajectory_Missile();
		}
		else if (IsRocket())
		{
			InitState_Trajectory_Straight();
		}
	}
	// 是否是对飞行器攻击
	AbstractClass* pTarget = nullptr;
	if (IsMissile() && (pTarget = pBullet->Target) != nullptr && (pTarget->WhatAmI() == AbstractType::Aircraft || pTarget->IsInAir()))
	{
		BulletExt::TargetAircraftBullets.push_back(pBullet);
	}
	_targetToAircraftFlag = true;
}

void BulletStatus::InitState_BlackHole() {};
void BulletStatus::InitState_ECM() {};

void BulletStatus::OnUpdate()
{
	if (!_targetToAircraftFlag)
	{
		_targetToAircraftFlag = true;
		// 是否是对飞行器攻击
		AbstractClass* pTarget = nullptr;
		if (IsMissile() && (pTarget = pBullet->Target) != nullptr && (pTarget->WhatAmI() == AbstractType::Aircraft || pTarget->IsInAir()))
		{
			BulletExt::TargetAircraftBullets.push_back(pBullet);
		}
	}
	// 弹道
	if (IsArcing())
	{
		OnUpdate_Trajectory_Arcing();
		OnUpdate_Trajectory_Bounce();
	}
	else if (IsRocket())
	{
		OnUpdate_Trajectory_Straight();
	}
	// 热诱弹
	OnUpdate_Trajectory_Decroy();

	// 自毁
	OnUpdate_DestroySelf();

	CoordStruct location = pBullet->GetCoords();
	// 潜地
	if (!life.IsDetonate && !HasPreImpactAnim(pBullet->WH))
	{
		if ((SubjectToGround || _isBounceSplit) && pBullet->GetHeight() < 0)
		{
			// 抛射体潜入地下，重新设置目标参数，并手动引爆
			CoordStruct targetPos = location;
			if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(location))
			{
				targetPos.Z = pTargetCell->GetCoordsWithBridge().Z;
				pBullet->SetTarget(pTargetCell);
			}
			pBullet->TargetCoords = targetPos;
			life.Detonate();
		}
		if (!life.IsDetonate && IsArcing() && pBullet->GetHeight() <= 8)
		{
			// Arcing 近炸
			CoordStruct tempSourcePos = location;
			tempSourcePos.Z = 0;
			CoordStruct tempTargetPos = pBullet->TargetCoords;
			tempTargetPos.Z = 0;
			if (tempSourcePos.DistanceFrom(tempTargetPos) <= static_cast<double>(256 + pBullet->Type->Acceleration))
			{
				// 距离目标太近，强制爆炸
				life.Detonate();
			}
		}
	}

	// 生命检查
	if (life.IsDetonate)
	{
		if (!life.IsHarmless)
		{
			pBullet->Detonate(location);
		}
		pBullet->UnInit();
		return;
	}
	if (life.Health <= 0)
	{
		life.IsDetonate = true;
	}
	// 其他逻辑
	if (!IsDeadOrInvisible(pBullet) && !life.IsDetonate)
	{
		OnUpdate_BlackHole();
		OnUpdate_ECM();
		OnUpdate_GiftBox();
		OnUpdate_RecalculateStatus();
		OnUpdate_SelfLaunchOrPumpAction();
	}
}

void BulletStatus::OnUpdate_BlackHole() {};
void BulletStatus::OnUpdate_ECM() {};
void BulletStatus::OnUpdate_RecalculateStatus() {};
void BulletStatus::OnUpdate_SelfLaunchOrPumpAction() {};

void BulletStatus::OnUpdateEnd()
{
	if (!IsDeadOrInvisible(pBullet) && !life.IsDetonate)
	{
		CoordStruct location = pBullet->GetCoords();
		OnUpdateEnd_BlackHole(location); // 黑洞会更新位置，要第一个执行
		OnUpdateEnd_Proximity(location);
	}
};

void BulletStatus::OnUpdateEnd_BlackHole(CoordStruct& sourcePos) {};

void BulletStatus::OnDetonate(CoordStruct* pCoords, bool& skip)
{
	if (pFakeTarget)
	{
		pFakeTarget->UnInit();
	}
	if (!skip)
	{
		if ((skip = OnDetonate_AntiBullet(pCoords)) == true)
		{
			return;
		}
		if ((skip = OnDetonate_Bounce(pCoords)) == true)
		{
			return;
		}
		if ((skip = OnDetonate_GiftBox(pCoords)) == true)
		{
			return;
		}
		if ((skip = OnDetonate_SelfLaunch(pCoords)) == true)
		{
			return;
		}
	}
};

bool BulletStatus::OnDetonate_SelfLaunch(CoordStruct* pCoords) { return false; };


TrajectoryData* BulletStatus::GetTrajectoryData()
{
	if (!_trajectoryData)
	{
		_trajectoryData = INI::GetConfig<TrajectoryData>(INI::Rules, pBullet->GetType()->ID)->Data;
	}
	return _trajectoryData;
}


