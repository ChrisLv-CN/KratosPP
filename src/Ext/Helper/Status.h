#pragma once

#include <GeneralDefinitions.h>
#include <AnimClass.h>
#include <BulletClass.h>
#include <HouseClass.h>
#include <TechnoClass.h>
#include <FootClass.h>

#include <Ext/AnimType/RelationData.h>

class AnimStatus;
class BulletStatus;
class CommonStatus;
class TechnoStatus;

class FilterData;

#pragma region AnimClass
AnimClass* SetAnimOwner(AnimClass* pAnim, HouseClass* pHouse);
AnimClass* SetAnimOwner(AnimClass* pAnim, TechnoClass* pTechno);
AnimClass* SetAnimOwner(AnimClass* pAnim, BulletClass* pBullet);

AnimClass* SetAnimCreater(AnimClass* pAnim, TechnoClass* pTechno);
AnimClass* SetAnimCreater(AnimClass* pAnim, BulletClass* pBullet);

void ShowAnim(AnimClass* pAnim, Relation visibility);
void HiddenAnim(AnimClass* pAnim);
#pragma endregion

#pragma region ObjectClass
bool IsDead(ObjectClass* pObject);
bool IsDeadOrInvisible(ObjectClass* pObject);
#pragma endregion

#pragma region TechnoClass
enum class DrivingState
{
	Moving = 0, Stand = 1, Start = 2, Stop = 3
};

enum class LocoType
{
	None = 0,
	Drive = 1,
	Hover = 2,
	Tunnel = 3,
	Walk = 4,
	Droppod = 5,
	Fly = 6,
	Teleport = 7,
	Mech = 8,
	Ship = 9,
	Jumpjet = 10,
	Rocket = 11
};

LocoType GetLocoType(TechnoClass* pTechno);

bool IsDead(TechnoClass* pTechno);
bool IsDeadOrInvisible(TechnoClass* pTechno);

bool IsCloaked(TechnoClass* pTechno, bool includeCloaking = true);
bool IsDeadOrInvisibleOrCloaked(TechnoClass* pTechno, bool includeCloaking = true);

bool IsImmune(TechnoClass* pTechno, bool checkStand = false);

bool AmIStand(TechnoClass* pTechno);

double GetROFMulti(TechnoClass* pTechno);
double GetDamageMulti(TechnoClass* pTechno);
int GetRangePlus(TechnoClass* pTechno, bool targetInAir = false);

void SetExtraSparkleAnim(TechnoClass* pTechno, AnimClass*& pAnim);
void ActiveRGBMode(TechnoClass* pTechno);

bool CanBeBase(TechnoClass* pTechno, bool eligibileForAllyBuilding, int houseIndex, int minX, int maxX, int minY, int maxY);

bool IsOnMark(TechnoClass* pTechno, FilterData data);

bool CanAttack(BulletClass* pBullet, TechnoClass* pTarget, bool isPassiveAcquire = false);
bool CanAttack(TechnoClass* pAttacker, AbstractClass* pTarget, bool inRange = false, int weaponIdx = -1, bool isPassiveAcquire = false);

int GetRealDamage(Armor armor, int damage, WarheadTypeClass* pWH, bool ignoreArmor = true, int distance = 0);

bool CanDamageMe(TechnoClass* pTechno, int damage, int distanceFromEpicenter, WarheadTypeClass* pWH, int& realDamage, bool effectsRequireDamage = false);
bool CanAffectMe(TechnoClass* pTechno, HouseClass* pAttackingHouse, WarheadTypeClass* pWH);

void ClearAllTarget(TechnoClass* pAttacker);
void ForceStopMoving(FootClass* pTechno);
void ForceStopMoving(ILocomotion* ploco);
#pragma endregion

#pragma region BulletClass
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

bool IsDead(BulletClass* pBullet, BulletStatus* status);
bool IsDead(BulletClass* pBullet);
bool IsDeadOrInvisible(BulletClass* pBullet, BulletStatus* status);
bool IsDeadOrInvisible(BulletClass* pBullet);
void SetSourceHouse(BulletClass* pBullet, HouseClass* pHouse);
/**
 *@brief 获取抛射体的当前所属，如果没有，则试图读取原始所属
 *
 * @param pBullet
 * @return HouseClass*
 */
HouseClass* GetHouse(BulletClass* pBullet);
/**
 * @brief 获取抛射体的原始所属，如果没有，则试图从发射者身上获取
 *
 * @param pBullet
 * @return HouseClass*
 */
HouseClass* GetSourceHouse(BulletClass* pBullet);

/**
 *@brief 计算抛射体当前位置和下一帧位置获得抛射体的飞行朝向
 *
 * @param pBullet
 * @param location
 * @return DirStruct
 */
DirStruct Facing(BulletClass* pBullet, CoordStruct location = CoordStruct::Empty);
#pragma endregion

#pragma region HouseClass
bool IsCivilian(HouseClass* pHouse);

Relation GetRelation(HouseClass* pHosue, HouseClass* pTargetHouse);
Relation GetRelationWithPlayer(HouseClass* pHouse);

bool AutoRepel(HouseClass* pHouse);

bool CanAffectHouse(HouseClass* pHouse, HouseClass* pTargetHouse, bool owner = true, bool allied = true, bool enemies = true, bool civilian = true);
#pragma endregion

#pragma region WarheadTypeClass
AnimClass* PlayWarheadAnim(WarheadTypeClass*, CoordStruct location, int damage = 1, LandType landType = LandType::Clear);
#pragma endregion

