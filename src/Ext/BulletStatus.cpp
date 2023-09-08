#include "BulletStatus.h"

#include <Extension/WarheadTypeExt.h>
#include <Ext/Helper.h>

std::vector<BulletClass*> BulletStatus::TargetAircraftBullets = {};

TrajectoryData* BulletStatus::GetTrajectoryData()
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

	INIBufferReader* reader = INI::GetSection(INI::Rules, _owner->GetType()->ID);
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

void BulletStatus::Destroy()
{
	auto it = std::find(TargetAircraftBullets.begin(), TargetAircraftBullets.end(), _owner);
	if (it != TargetAircraftBullets.end())
	{
		TargetAircraftBullets.erase(it);
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
	AbstractClass *pTarget = nullptr;
	if (IsMissile() && (pTarget = _owner->Target) != nullptr && (pTarget->What_Am_I() == AbstractType::Aircraft || pTarget->IsInAir()))
	{
		TargetAircraftBullets.push_back(_owner);
	}
}

void BulletStatus::InitState_BlackHole() {};
void BulletStatus::InitState_Bounce() {};
void BulletStatus::InitState_DestroySelf() {};
void BulletStatus::InitState_ECM() {};
void BulletStatus::InitState_GiftBox() {};
void BulletStatus::InitState_Paintball() {};
void BulletStatus::InitState_Proximity() {};

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
			if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(location))
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
			if (tempSoucePos.DistanceFrom(tempTargetPos) <= static_cast<double>(256 + _owner->Type->Acceleration))
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

void BulletStatus::OnUpdate_DestroySelf() {};

void BulletStatus::OnUpdate_BlackHole() {};
void BulletStatus::OnUpdate_ECM() {};
void BulletStatus::OnUpdate_GiftBox() {};
void BulletStatus::OnUpdate_RecalculateStatus() {};
void BulletStatus::OnUpdate_SelfLaunchOrPumpAction() {};

void BulletStatus::OnUpdateEnd()
{
	if (!IsDeadOrInvisible(_owner) && !life.IsDetonate)
	{
		CoordStruct location = _owner->GetCoords();
		OnUpdateEnd_BlackHole(location); // 黑洞会更新位置，要第一个执行
		OnUpdateEnd_Proximity(location);
	}
};

void BulletStatus::OnUpdateEnd_BlackHole(CoordStruct& sourcePos) {};
void BulletStatus::OnUpdateEnd_Proximity(CoordStruct& sourcePos) {};

void BulletStatus::OnDetonate(CoordStruct* pCoords, bool& skip)
{
	if (pFakeTarget)
	{
		pFakeTarget->UnInit();
	}
	if (!skip)
	{
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

bool BulletStatus::OnDetonate_Bounce(CoordStruct* pCoords) { return false; };
bool BulletStatus::OnDetonate_GiftBox(CoordStruct* pCoords) { return false; };
bool BulletStatus::OnDetonate_SelfLaunch(CoordStruct* pCoords) { return false; };



// ----------------
// Helper
// ----------------

BulletType WhatTypeAmI(BulletClass* pBullet)
{
	BulletTypeClass* pType = nullptr;
	if (pBullet && (pType = pBullet->Type) != nullptr)
	{
		if (pType->Inviso)
		{
			// Inviso 优先级最高
			return BulletType::INVISO;
		}
		else if (pType->ROT > 0)
		{
			// 导弹类型
			if (pType->ROT == 1)
			{
				return BulletType::ROCKET;
			}
			return BulletType::MISSILE;
		}
		else if (pType->Vertical)
		{
			// 炸弹
			return BulletType::BOMB;
		}
		else if (pType->Arcing)
		{
			// 最后是Arcing
			return BulletType::ARCING;
		}
		else if (pType->ROT == 0)
		{
			// 再然后还有一个ROT=0的抛物线，但不是Arcing
			return BulletType::NOROT;
		}
	}
	return BulletType::UNKNOWN;
}

// ----------------
// 高级弹道学
// ----------------

CoordStruct GetInaccurateOffset(float scatterMin, float scatterMax)
{
	// 不精确, 需要修改目标坐标
	int min = (int)(scatterMin * 256);
	int max = scatterMax > 0 ? (int)(scatterMax * 256) : RulesClass::Instance->BallisticScatter;
	// Logger.Log("炮弹[{0}]不精确, 需要重新计算目标位置, 散布范围=[{1}, {2}]", pBullet.Ref.Type.Convert<AbstractTypeClass>().Ref.ID, min, max);
	if (min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}
	// 随机偏移
	return RandomOffset(min, max);
}

BulletVelocity GetBulletArcingVelocity(CoordStruct sourcePos, CoordStruct targetPos,
	double speed, double gravity, bool lobber,
	int zOffset, double& straightDistance, double& realSpeed)
{
	// 重算抛物线弹道
	if (gravity == 0)
	{
		gravity = RulesClass::Instance->Gravity;
	}
	CoordStruct tempSourcePos = sourcePos;
	CoordStruct tempTargetPos = targetPos;
	int zDiff = tempTargetPos.Z - tempSourcePos.Z + zOffset; // 修正高度差
	tempTargetPos.Z = 0;
	tempSourcePos.Z = 0;
	straightDistance = tempTargetPos.DistanceFrom(tempSourcePos);
	// Logger.Log("位置和目标的水平距离{0}", straightDistance);
	realSpeed = speed;
	if (straightDistance == 0 || std::isnan(straightDistance))
	{
		// 直上直下
		return BulletVelocity{ 0.0, 0.0, gravity };
	}
	if (realSpeed == 0)
	{
		// realSpeed = WeaponTypeClass.GetSpeed((int)straightDistance, gravity);
		realSpeed = Math::sqrt(straightDistance * gravity * 1.2);
		// Logger.Log($"YR计算的速度{realSpeed}, 距离 {(int)straightDistance}, 重力 {gravity}");
	}
	// 高抛弹道
	if (lobber)
	{
		realSpeed = (int)(realSpeed * 0.5);
		// Logger.Log("高抛弹道, 削减速度{0}", realSpeed);
	}
	// Logger.Log("重新计算初速度, 当前速度{0}", realSpeed);
	double vZ = (zDiff * realSpeed) / straightDistance + 0.5 * gravity * straightDistance / realSpeed;
	// Logger.Log("计算Z方向的初始速度{0}", vZ);
	BulletVelocity v(tempTargetPos.X - tempSourcePos.X, tempTargetPos.Y - tempSourcePos.Y, 0.0);
	v *= realSpeed / straightDistance;
	v.Z = vZ;
	return v;
}

BulletVelocity GetBulletArcingVelocity(CoordStruct sourcePos, CoordStruct& targetPos,
	double speed, double gravity, bool lobber, bool inaccurate, float scatterMin, float scatterMax,
	int zOffset, double& straightDistance, double& realSpeed, CellClass*& pTargetCell)
{
	// 不精确
	if (inaccurate)
	{
		targetPos += GetInaccurateOffset(scatterMin, scatterMax);
	}
	// 不潜地
	if ((pTargetCell = MapClass::Instance->TryGetCellAt(targetPos)) != nullptr)
	{
		targetPos.Z = pTargetCell->GetCoordsWithBridge().Z;
	}
	return GetBulletArcingVelocity(sourcePos, targetPos, speed, gravity, lobber, zOffset, straightDistance, realSpeed);
}

