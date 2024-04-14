#include "Weapon.h"
#include "CastEx.h"
#include "DrawEx.h"
#include "FLH.h"
#include "MathEx.h"
#include "Scripts.h"
#include "Status.h"

#include <SpawnManagerClass.h>
#include <VocClass.h>
#include <WarheadTypeClass.h>

#include <Extension/TechnoExt.h>
#include <Extension/BulletExt.h>
#include <Extension/WeaponTypeExt.h>

#include <Ext/ObjectType/AttachEffect.h>
#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/WeaponType/TargetLaserData.h>

// ----------------
// 高级弹道学
// ----------------
#pragma region Advanced Ballistics
BulletVelocity GetBulletVelocity(CoordStruct source, CoordStruct target)
{
	CoordStruct flh{ 1, 0, 0 };
	DirStruct dir = Point2Dir(source, target);
	CoordStruct v = GetFLHAbsoluteOffset(flh, dir);
	return ToVelocity(v);
}

BulletVelocity RecalculateBulletVelocity(BulletClass* pBullet, CoordStruct source, CoordStruct target)
{
	CoordStruct vector = target - source;
	BulletVelocity v = ToVelocity(vector);
	double dist = target.DistanceFrom(source);
	v *= isnan(dist) || dist <= 0 ? 0 : (pBullet->Speed / dist);
	pBullet->Velocity = v;
	pBullet->SourceCoords = source;
	pBullet->TargetCoords = target;
	return v;
}

BulletVelocity RecalculateBulletVelocity(BulletClass* pBullet, CoordStruct target)
{
	return RecalculateBulletVelocity(pBullet, pBullet->GetCoords(), target);
}

BulletVelocity RecalculateBulletVelocity(BulletClass* pBullet)
{
	return RecalculateBulletVelocity(pBullet, pBullet->GetCoords(), pBullet->GetTargetCoords());
}

CoordStruct GetInaccurateOffset(float scatterMin, float scatterMax)
{
	// 不精确, 需要修改目标坐标
	int min = (int)(scatterMin * 256);
	int max = scatterMax > 0 ? (int)(scatterMax * 256) : RulesClass::Instance->BallisticScatter;
	if (min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}
	// 随机偏移
	return GetRandomOffset(min, max);
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
	}
	// 高抛弹道
	if (lobber)
	{
		realSpeed = (int)(realSpeed * 0.5);
	}
	double vZ = (zDiff * realSpeed) / straightDistance + 0.5 * gravity * straightDistance / realSpeed;
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
#pragma endregion

// ----------------
// 自定义武器发射
// ----------------
#pragma region Custom fire weapon

RadialFire::RadialFire(DirStruct dir, int burst, int splitAngle)
{
	this->burst = burst;
	InitData(dir, splitAngle);
}

void RadialFire::InitData(DirStruct dir, int splitAngle)
{
	dirRad = dir.GetRadian();
	splitRad = Math::deg2rad(splitAngle * 0.5); // Deg2Rad是逆时针
	delta = splitAngle / (burst + 1);
	deltaZ = 1.0f / (burst / 2.0f + 1);
}

BulletVelocity RadialFire::GetBulletVelocity(int index, bool radialZ)
{
	int z = 0;
	float temp = burst / 2.0f;
	if (radialZ)
	{
		if (index - temp < 0)
		{
			z = index;
		}
		else
		{
			z = std::abs(index - burst + 1);
		}
	}
	int angle = delta * (index + 1);
	double radians = Math::deg2rad(angle); // 逆时针
	Matrix3D mtx;
	mtx.MakeIdentity();
	mtx.RotateZ(static_cast<float>(dirRad)); // 转到单位朝向
	mtx.RotateZ(static_cast<float>(splitRad)); // 逆时针转到发射角
	mtx.RotateZ(-static_cast<float>(radians)); // 顺时针发射
	mtx.Translate(256, 0, 0);
	Vector3D<float> offset = Matrix3D::MatrixMultiply(mtx, Vector3D<float>::Empty);
	return { offset.X, -offset.Y, deltaZ * z };
}

TechnoClass* WhoIsShooter(TechnoClass* pAttacker)
{
	TechnoClass* pTransporter = pAttacker->Transporter;
	if (pTransporter)
	{
		// I'm a passengers
		pAttacker = WhoIsShooter(pTransporter);
	}
	return pAttacker;
}


bool InRange(ObjectClass* pObject, AbstractClass* pTarget, WeaponTypeClass* pWeapon, int minRange, int maxRange)
{
	CoordStruct location = pObject->GetCoords();
	switch (pObject->WhatAmI())
	{
	case AbstractType::Building:
	case AbstractType::Infantry:
	case AbstractType::Unit:
	case AbstractType::Aircraft:
		return dynamic_cast<TechnoClass*>(pObject)->InRange(&location, pTarget, pWeapon);
	default:
		CoordStruct targetPos = pTarget->GetCoords();
		double distance = targetPos.DistanceFrom(location);
		if (isnan(distance))
		{
			distance = 0;
		}
		return distance <= pWeapon->Range && distance >= minRange;
	}
}


void FireWeaponTo(TechnoClass* pShooter, TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse,
	WeaponTypeClass* pWeapon, CoordStruct flh, bool isOnTurret,
	FireBulletToTarget callback, CoordStruct bulletSourcePos,
	bool radialFire, int splitAngle, bool radialZ)
{
	if (!pTarget)
	{
		return;
	}
	CoordStruct targetPos = pTarget->GetCoords();
	// radial fire
	int burst = pWeapon->Burst;
	DirStruct dir;
	if (pShooter)
	{
		dir = pShooter->GetRealFacing().Desired();
		if (pShooter->HasTurret())
		{
			dir = pShooter->TurretFacing().Desired();
		}
	}
	RadialFire radial{ dir, burst, splitAngle };
	int flipY = -1;
	for (int i = 0; i < burst; i++)
	{
		BulletVelocity bulletVelocity;
		if (radialFire)
		{
			flipY = (i < burst / 2.0) ? -1 : 1;
			bulletVelocity = radial.GetBulletVelocity(i, radialZ);
		}
		else
		{
			flipY *= -1;
		}
		CoordStruct sourcePos = bulletSourcePos;
		if (sourcePos.IsEmpty())
		{
			// get flh
			sourcePos = GetFLHAbsoluteCoords(pShooter, flh, isOnTurret, flipY, CoordStruct::Empty);
		}
		if (bulletVelocity.IsEmpty())
		{
			bulletVelocity = GetBulletVelocity(sourcePos, targetPos);
		}
		CoordStruct tempFLH = flh;
		tempFLH.Y *= flipY;
		BulletClass* pBullet = FireBulletTo(pShooter, pAttacker, pTarget, pAttacingHouse, pWeapon, sourcePos, targetPos, bulletVelocity, tempFLH, isOnTurret);
		// callback
		if (callback && pBullet)
		{
			callback(i, burst, pBullet, pTarget);
		}
	}
}

BulletClass* FireBulletTo(ObjectClass* pShooter, TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse,
	WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos, BulletVelocity velocity, CoordStruct flh, bool isOnTurret)
{
	TechnoClass* pTargetTechno = nullptr;
	if (!pTarget || (CastToTechno(pTarget, pTargetTechno) && IsDeadOrInvisible(pTargetTechno)))
	{
		return nullptr;
	}
	// Get fireMulti
	double fireMulti = 1;
	if (!IsDead(pAttacker))
	{
		// check spawner
		SpawnManagerClass* pSpawns = pAttacker->SpawnManager;
		if (pWeapon->Spawner && pSpawns)
		{
			pSpawns->SetTarget(pTarget);
			return nullptr;
		}
		// check Abilities FIREPOWER
		fireMulti = GetDamageMulti(pAttacker);
	}
	// Fire Weapon
	BulletClass* pBullet = FireBullet(pAttacker, pTarget, pAttacingHouse, pWeapon, fireMulti, sourcePos, targetPos, velocity);
	// Draw bullet effect
	DrawBulletEffect(pWeapon, sourcePos, targetPos, pAttacker, pTarget, pAttacingHouse, flh, isOnTurret);
	// Draw particle system
	AttachedParticleSystem(pWeapon, sourcePos, targetPos, pAttacker, pTarget, pAttacingHouse);
	// Play report sound
	PlayReportSound(pWeapon, sourcePos);
	if (pShooter)
	{
		// Draw weapon anim
		DrawWeaponAnim(pShooter, pAttacker, pAttacingHouse, pWeapon, sourcePos, targetPos);
		// feedbackAE
		TechnoClass* pShooterT = nullptr;
		BulletClass* pShooterB = nullptr;
		AttachEffect* pAEM = nullptr;
		if ((CastToTechno(pShooter, pShooterT) && TryGetAEManager<TechnoExt>(pShooterT, pAEM))
			|| (CastToBullet(pShooter, pShooterB) && TryGetAEManager<BulletExt>(pShooterB, pAEM))
		)
		{
			pAEM->FeedbackAttach(pWeapon);
		}
		if (pShooterT)
		{
			TechnoStatus* status = nullptr;
			if (TryGetStatus<TechnoExt>(pShooterT, status))
			{
				status->RockerPitch(pWeapon);
				// Draw target laser
				TargetLaserData* data = INI::GetConfig<TargetLaserData>(INI::Rules, pWeapon->ID)->Data;
				if (data->Enable)
				{
					if (!data->BreakTargetLaser)
					{
						status->StartTargetLaser(pTarget, pWeapon, *data, flh, isOnTurret);
					}
					else
					{
						status->CloseTargetLaser(pTarget);
					}
				}
			}
		}
	}
	return pBullet;
}

BulletClass* FireBullet(TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse,
	WeaponTypeClass* pWeapon, double fireMulti,
	CoordStruct sourcePos, CoordStruct targetPos, BulletVelocity velocity)
{
	BulletTypeClass* pBulletType = pWeapon->Projectile;
	if (pBulletType)
	{
		int damage = static_cast<int>(pWeapon->Damage * fireMulti);
		WarheadTypeClass* pWH = pWeapon->Warhead;
		int speed = pWeapon->GetSpeed(sourcePos, targetPos);
		bool bright = pWeapon->Bright; // 原游戏中弹头上的bright是无效的
		BulletClass* pBullet = pBulletType->CreateBullet(pTarget, pAttacker, damage, pWH, speed, bright);
		pBullet->WeaponType = pWeapon;
		// 记录所属
		SetSourceHouse(pBullet, pAttacingHouse);
		if (velocity.IsEmpty())
		{
			velocity = GetBulletVelocity(sourcePos, targetPos);
		}
		pBullet->MoveTo(sourcePos, velocity);
		if (!targetPos.IsEmpty())
		{
			pBullet->TargetCoords = targetPos;
		}
		return pBullet;
	}
	return nullptr;
}

void DrawBulletEffect(WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos,
	TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse, CoordStruct flh, bool isOnTurret)
{
	// IsLaser
	if (pWeapon->IsLaser)
	{
		LaserType laser;
		ColorStruct houseColor;
		if (pWeapon->IsHouseColor)
		{
			laser.IsHouseColor = true;
			if (pAttacker)
			{
				houseColor = pAttacker->Owner->LaserColor;
			}
			else if (pAttacingHouse)
			{
				houseColor = pAttacingHouse->LaserColor;
			}
		}
		laser.InnerColor = pWeapon->LaserInnerColor;
		laser.OuterColor = pWeapon->LaserOuterColor;
		laser.OuterSpread = pWeapon->LaserOuterSpread;
		laser.IsHouseColor = pWeapon->IsHouseColor;
		laser.Duration = pWeapon->LaserDuration;
		// get thickness and fade
		WeaponTypeExt::TypeData* data = GetTypeData<WeaponTypeExt, WeaponTypeExt::TypeData>(pWeapon);
		if (data->LaserThickness > 0)
		{
			laser.Thickness = data->LaserThickness;
		}
		laser.Fade = data->LaserFade;
		laser.IsSupported = data->IsSupported;
		// draw the laser
		DrawLaser(laser, sourcePos, targetPos, houseColor);
	}
	// IsRadBeam
	if (pWeapon->IsRadBeam)
	{
		RadBeamType beamType = RadBeamType::RadBeam;
		if (pWeapon->Warhead && pWeapon->Warhead->Temporal)
		{
			beamType = RadBeamType::Temporal;
		}
		BeamType type{ beamType };
		DrawBeam(sourcePos, targetPos, type);
	}
	// IsElectricBolt
	if (pWeapon->IsElectricBolt)
	{
		if (pAttacker && pTarget)
		{
			DrawBolt(pAttacker, pTarget, pWeapon, sourcePos, flh, isOnTurret);
		}
		else
		{
			DrawBolt(sourcePos, targetPos, pWeapon->IsAlternateColor);
		}
	}
}

void AttachedParticleSystem(WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos,
	TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse)
{
	if (ParticleSystemTypeClass* psType = pWeapon->AttachedParticleSystem)
	{
		DrawParticle(psType, sourcePos, targetPos, pAttacker, pTarget);
	}
}

void PlayReportSound(WeaponTypeClass* pWeapon, CoordStruct sourcePos)
{
	if (pWeapon->Report.Count > 0)
	{
		int index = Random::RandomRanged(0, pWeapon->Report.Count - 1);
		int soundIndex = pWeapon->Report.GetItem(index);
		if (soundIndex != -1)
		{
			VocClass::PlayAt(soundIndex, sourcePos);
		}
	}
}

void DrawWeaponAnim(ObjectClass* pShooter, TechnoClass* pAttacker, HouseClass* pAttackingHouse, WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos)
{
	// Anim
	if (pWeapon->Anim.Count > 0)
	{
		int facing = pWeapon->Anim.Count;
		int index = 0;
		if (facing % 8 == 0)
		{
			CoordStruct tempSourcePos = sourcePos;
			tempSourcePos.Z = 0;
			CoordStruct tempTargetPos = targetPos;
			tempTargetPos.Z = 0;
			DirStruct dir = Point2Dir(tempSourcePos, tempTargetPos);
			index = Dir2FrameIndex(dir, facing);
		}
		AnimTypeClass* pAnimType = pWeapon->Anim.GetItem(index);
		if (pAnimType)
		{
			AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, sourcePos);
			SetAnimOwner(pAnim, pAttackingHouse);
			if (!pAnimType->Bouncer && !pAnimType->IsMeteor)
			{
				pAnim->SetOwnerObject(pShooter);
			}
		}
	}
}
#pragma endregion
