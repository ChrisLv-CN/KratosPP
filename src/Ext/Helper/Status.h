#pragma once

#include <GeneralStructures.h>
#include <AnimClass.h>
#include <BulletClass.h>
#include <HouseClass.h>
#include <TechnoClass.h>

#pragma region AnimClass
AnimClass* SetAnimOwner(AnimClass* pAnim, HouseClass* pHouse);
AnimClass* SetAnimOwner(AnimClass* pAnim, TechnoClass* pTechno);
AnimClass* SetAnimOwner(AnimClass* pAnim, BulletClass* pBullet);

AnimClass* SetAnimCreater(AnimClass* pAnim, TechnoClass* pTechno);
AnimClass* SetAnimCreater(AnimClass* pAnim, BulletClass* pBullet);
#pragma endregion

#pragma region ObjectClass
bool IsDead(ObjectClass* pObject);
bool IsDeadOrInvisible(ObjectClass* pObject);
#pragma endregion

#pragma region TechnoClass
bool IsDead(TechnoClass* pTechno);
bool IsDeadOrInvisible(TechnoClass* pTechno);

bool IsCloaked(TechnoClass* pTechno, bool includeCloaking = true);
bool IsDeadOrInvisibleOrCloaked(TechnoClass* pTechno, bool includeCloaking = true);

double GetROFMulti(TechnoClass* pTechno);
double GetDamageMulti(TechnoClass* pTechno);

void SetExtraSparkleAnim(TechnoClass* pTechno, AnimClass*& pAnim);
void ActiveRGBMode(TechnoClass* pTechno);

bool CanAttack(TechnoClass* pAttacker, AbstractClass* pTarget, bool isPassiveAcquire = false);
#pragma endregion

enum class BulletType
{
	UNKNOWN = 0,
	INVISO = 1,
	ARCING = 2,
	MISSILE = 3,
	ROCKET = 4,
	NOROT = 5,
	BOMB = 6
};

#pragma region BulletClass
/** @brief 获取抛射体的轨迹类型.
 *
 * Inviso优先级最高；\n
 * Arcing 和 ROT>0 一起写，无法发射；\n
 * Arcing 和 ROT=0 一起写，是抛物线；\n
 * Arcing 和 Vertical 一起写，无法发射；\n
 * ROT>0 和 Vertical 一起写，是导弹；\n
 * ROT=0 和 Vertical 一起写，是垂直，SHP会变直线导弹，VXL会垂直下掉。\n
 */
BulletType WhatAmI(BulletClass* pBullet);

bool IsDead(BulletClass* pBullet);
bool IsDeadOrInvisible(BulletClass* pBullet);
void SetSourceHouse(BulletClass* pBullet, HouseClass* pHouse);

DirStruct Facing(BulletClass* pBullet, CoordStruct location = CoordStruct::Empty);
#pragma endregion

#pragma region HouseClass
bool IsCivilian(HouseClass* pHouse);
#pragma endregion
