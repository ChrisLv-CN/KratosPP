#include "BulletStatus.h"

#include <Extension/WarheadTypeExt.h>

std::vector<BulletClass *> BulletStatus::TargetAircraftBullets = {};

TrajectoryData *BulletStatus::GetTrajectoryData()
{
	if (!_trajectoryData)
	{
		_trajectoryData = INI::GetConfig<TrajectoryData>(INI::Rules, _owner->GetType()->ID)->Data;
	}
	return _trajectoryData;
}

BulletType BulletStatus::GetBulletType()
{
	if (_bulletType == BulletType::UNKNOWN)
	{
		_bulletType = WhatTypeAmI(_owner);
		if (_bulletType != BulletType::ROCKET && GetTrajectoryData()->IsStraight())
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
	pSource = _owner->Owner;
	if (pSource)
	{
		pSourceHouse = pSource->Owner;
	}

	// 读取生命值
	int health = _owner->Health;
	// 抛射体武器伤害为复述或者零时需要处理
	if (health < 0)
	{
		health = -health;
	}
	else if (health == 0)
	{
		health = 1; // 武器伤害为0，如[NukeCarrier]
	}

	INIBufferReader *reader = INI::GetSection(INI::Rules, _owner->GetType()->ID);
	// 初始化生命
	this->life.Health = health;
	this->life.Read(reader);
	// 初始化伤害
	this->damage.Damage = health;
	// 初始化地面碰撞属性
	switch (GetTrajectoryData()->SubjectToGround)
	{
	case SubjectToGroundType::YES:
		this->SubjectToGround = true;
		break;
	case SubjectToGroundType::NO:
		this->SubjectToGround = false;
		break;
	default:
		this->SubjectToGround = !IsArcing() && !IsRocket() && !GetTrajectoryData()->IsStraight();
		break;
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

void BulletStatus::ResetTarget(AbstractClass *pNewTarget, CoordStruct targetPos)
{
	_owner->SetTarget(pNewTarget);
	if (targetPos == CoordStruct::Empty && pNewTarget)
	{
		targetPos = pNewTarget->GetCoords();
	}
	_owner->TargetCoords = targetPos;
	// 重设弹道
	if (IsArcing())
	{
		CoordStruct sourcePos = _owner->GetCoords();
		_owner->SourceCoords = sourcePos;
		ResetArcingVelocity(1.0f, true);
	}
	else if (IsRocket())
	{
		CoordStruct sourcePos = _owner->GetCoords();
		_owner->SourceCoords = sourcePos;
		InitState_Trajectory_Straight();
	}
}

void BulletStatus::OnPut(CoordStruct *pLocation, DirType dir)
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
	AbstractClass *pTarget = nullptr;
	if (IsMissile() && (pTarget = _owner->Target) && (pTarget->What_Am_I() == AbstractType::Aircraft || pTarget->IsInAir()))
	{
		TargetAircraftBullets.push_back(_owner);
	}
}

void BulletStatus::InitState_BlackHole(){};
void BulletStatus::InitState_Bounce(){};
void BulletStatus::InitState_DestroySelf(){};
void BulletStatus::InitState_ECM(){};
void BulletStatus::InitState_GiftBox(){};
void BulletStatus::InitState_Paintball(){};
void BulletStatus::InitState_Proximity(){};

void BulletStatus::OnUpdate()
{
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

	CoordStruct location = _owner->GetCoords();
	// 潜地
	if (!life.IsDetonate && !HasPreImpactAnim(_owner->WH))
	{
		if ((SubjectToGround || IsBounceSplit) && _owner->GetHeight() < 0)
		{
			// 抛射体潜入地下，重新设置目标参数，并手动引爆
			CoordStruct targetPos = location;
			if (CellClass *pTargetCell = MapClass::Instance->TryGetCellAt(location))
			{
				targetPos.Z = pTargetCell->GetCoordsWithBridge().Z;
				_owner->SetTarget(pTargetCell);
			}
			_owner->TargetCoords = targetPos;
			life.Detonate();
		}
		if (!life.IsDetonate && IsArcing() && _owner->GetHeight() <= 8)
		{
			// Arcing 近炸
			CoordStruct tempSoucePos = location;
			tempSoucePos.Z = 0;
			CoordStruct tempTargetPos = _owner->TargetCoords;
			tempTargetPos.Z = 0;
			if (tempSoucePos.DistanceFrom(tempTargetPos) <= 256 + _owner->Type->Acceleration)
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
			_owner->Detonate(location);
		}
		_owner->UnInit();
		return;
	}
	if (life.Health <= 0)
	{
		life.IsDetonate = true;
	}
	// 其他逻辑
	if (!IsDeadOrInvisible(_owner) && !life.IsDetonate)
	{
		OnUpdate_BlackHole();
		OnUpdate_ECM();
		OnUpdate_GiftBox();
		OnUpdate_RecalculateStatus();
		OnUpdate_SelfLaunchOrPumpAction();
	}
}

void BulletStatus::OnUpdate_DestroySelf(){};

void BulletStatus::OnUpdate_BlackHole(){};
void BulletStatus::OnUpdate_ECM(){};
void BulletStatus::OnUpdate_GiftBox(){};
void BulletStatus::OnUpdate_RecalculateStatus(){};
void BulletStatus::OnUpdate_SelfLaunchOrPumpAction(){};

void BulletStatus::OnUpdateEnd()
{
	if (!IsDeadOrInvisible(_owner) && !life.IsDetonate)
	{
		CoordStruct location = _owner->GetCoords();
		OnUpdateEnd_BlackHole(location); // 黑洞会更新位置，要第一个执行
		OnUpdateEnd_Proximity(location);
	}
};

void BulletStatus::OnUpdateEnd_BlackHole(CoordStruct &sourcePos){};
void BulletStatus::OnUpdateEnd_Proximity(CoordStruct &sourcePos){};

void BulletStatus::OnDetonate(CoordStruct *pCoords, bool &skip)
{
	if (pFakeTarget)
	{
		pFakeTarget->UnInit();
	}
	if (!skip)
	{
		if (skip = OnDetonate_Bounce(pCoords))
		{
			return;
		}
		if (skip = OnDetonate_GiftBox(pCoords))
		{
			return;
		}
		if (skip = OnDetonate_SelfLaunch(pCoords))
		{
			return;
		}
	}
};

bool BulletStatus::OnDetonate_Bounce(CoordStruct *pCoords) { return false; };
bool BulletStatus::OnDetonate_GiftBox(CoordStruct *pCoords) { return false; };
bool BulletStatus::OnDetonate_SelfLaunch(CoordStruct *pCoords) { return false; };

void BulletStatus::OnUnInit()
{
	auto it = std::find(TargetAircraftBullets.begin(), TargetAircraftBullets.end(), _owner);
	if (it != TargetAircraftBullets.end())
	{
		TargetAircraftBullets.erase(it);
	}
}
