#pragma once

#include <GeneralDefinitions.h>
#include <BulletClass.h>

// ----------------
// 高级弹道学
// ----------------
#pragma region Advanced Ballistics
BulletVelocity GetBulletVelocity(CoordStruct source, CoordStruct target);

BulletVelocity RecalculateBulletVelocity(BulletClass* pBullet, CoordStruct source, CoordStruct target);

BulletVelocity RecalculateBulletVelocity(BulletClass* pBullet, CoordStruct target);

BulletVelocity RecalculateBulletVelocity(BulletClass* pBullet);

/// @brief 获取不精确落点的散布偏移值.
/// @param scatterMin 最小散布范围，单位格
/// @param scatterMax 最大散布范围，单位格
/// @return offset
CoordStruct GetInaccurateOffset(float scatterMin, float scatterMax);

/// @brief 计算抛物线弹道的出膛速度向量.
/// @param sourcePos 炮弹初始位置
/// @param targetPos 炮弹目标位置
/// @param speed 速度
/// @param gravity 重力
/// @param lobber 是否高抛
/// @param zOffset 高度修正
/// @param straightDistance out 距离
/// @param realSpeed out 计算后的实际速度
/// @return velocity
BulletVelocity GetBulletArcingVelocity(CoordStruct sourcePos, CoordStruct targetPos,
	double speed, double gravity, bool lobber,
	int zOffset, double& straightDistance, double& realSpeed);

/// @brief 计算抛物线弹道的出膛速度向量.
/// @param sourcePos 炮弹初始位置
/// @param targetPos out 炮弹目标位置
/// @param speed 速度
/// @param gravity 重力
/// @param lobber 是否高抛
/// @param inaccurate 是否不精确散布
/// @param scatterMin 散布最小范围
/// @param scatterMax 散布最大范围
/// @param zOffset 高度修正
/// @param straightDistance out 距离
/// @param realSpeed out 实际速度
/// @param pTargetCell out 目标格子
/// @return velocity
BulletVelocity GetBulletArcingVelocity(CoordStruct sourcePos, CoordStruct& targetPos,
	double speed, double gravity, bool lobber, bool inaccurate, float scatterMin, float scatterMax,
	int zOffset, double& straightDistance, double& realSpeed, CellClass*& pTargetCell);
#pragma endregion

// ----------------
// 自定义武器发射
// ----------------
#pragma region Custom fire weapon
typedef void (*FireBulletToTarget)(int index, int burst, BulletClass*& pBullet, AbstractClass*& pTarget);

class RadialFire
{
public:
	RadialFire(DirStruct dir, int burst, int splitAngle);

	void InitData(DirStruct dir, int splitAngle);

	BulletVelocity GetBulletVelocity(int index, bool radialZ);

private:
	int burst = 1;
	double dirRad = 0;
	double splitRad = 0;
	int delta = 0;
	float deltaZ = 0;
};



TechnoClass* WhoIsShooter(TechnoClass* pAttacker);

bool InRange(ObjectClass* pObject, AbstractClass* pTarget, WeaponTypeClass* pWeapon, int minRange, int maxRange);

void FireWeaponTo(TechnoClass* pShooter, TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse,
	WeaponTypeClass* pWeapon, CoordStruct flh, bool isOnTurret,
	FireBulletToTarget callback = nullptr, CoordStruct bulletSourcePos = CoordStruct::Empty,
	bool radialFire = false, int splitAngle = 180, bool radialZ = true);

BulletClass* FireBulletTo(ObjectClass* pShooter, TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse,
	WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos, BulletVelocity velocity = BulletVelocity::Empty, CoordStruct flh = CoordStruct::Empty, bool isOnTurret = true);

BulletClass* FireBullet(TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse,
	WeaponTypeClass* pWeapon, double fireMulti = 1.0,
	CoordStruct sourcePos = CoordStruct::Empty, CoordStruct targetPos = CoordStruct::Empty, BulletVelocity velocity = BulletVelocity::Empty);

void DrawBulletEffect(WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos,
	TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse, CoordStruct flh, bool isOnTurret);

void AttachedParticleSystem(WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos,
	TechnoClass* pAttacker, AbstractClass* pTarget, HouseClass* pAttacingHouse);

void PlayReportSound(WeaponTypeClass* pWeapon, CoordStruct sourcePos);

void DrawWeaponAnim(ObjectClass* pShooter, TechnoClass* pAttacker, HouseClass* pAttackingHouse, WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct targetPos);
#pragma endregion
