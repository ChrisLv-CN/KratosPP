﻿#include "Status.h"
#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/AnimStatus.h>
#include <Ext/BulletStatus.h>
#include <Ext/CommonStatus.h>
#include <Ext/TechnoStatus.h>

#include <Extension/WarheadTypeExt.h>


#pragma endregion AnimClass
AnimClass* SetAnimOwner(AnimClass* pAnim, HouseClass* pHouse)
{
	pAnim->Owner = pHouse;
	return pAnim;
}

AnimClass* SetAnimOwner(AnimClass* pAnim, TechnoClass* pTechno)
{
	pAnim->Owner = pTechno->Owner;
	return pAnim;
}

AnimClass* SetAnimOwner(AnimClass* pAnim, BulletClass* pBullet)
{
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		pAnim->Owner = status->pSourceHouse;
	}
	return pAnim;
}

AnimClass* SetAnimCreater(AnimClass* pAnim, TechnoClass* pTechno)
{
	if (AnimStatus* status = GetStatus<AnimExt, AnimStatus>(pAnim))
	{
		status->pCreater = pTechno;
	}
	return pAnim;
}

AnimClass* SetAnimCreater(AnimClass* pAnim, BulletClass* pBullet)
{
	TechnoClass* Source = pBullet->Owner;
	if (!IsDead(Source))
	{
		if (AnimStatus* status = GetStatus<AnimExt, AnimStatus>(pAnim))
		{
			status->pCreater = Source;
		}
	}
	return pAnim;
}
#pragma endregion

#pragma region ObjectClass
bool IsDead(ObjectClass* pObject)
{
	// if pObject is a bullet, the Health maybe < 0
	return !pObject || !pObject->GetType() || pObject->Health == 0 || !pObject->IsAlive;
}
bool IsDeadOrInvisible(ObjectClass* pObject)
{
	return IsDead(pObject) || pObject->InLimbo;
}
#pragma endregion

#pragma endregion TechnoClass
bool IsDead(TechnoClass* pTechno)
{
	return !pTechno || !pTechno->GetType() || pTechno->Health <= 0 || !pTechno->IsAlive || pTechno->IsCrashing || pTechno->IsSinking;
}

bool IsDeadOrInvisible(TechnoClass* pTechno)
{
	return IsDead(pTechno) || pTechno->InLimbo;
}

bool IsCloaked(TechnoClass* pTechno, bool includeCloaking)
{
	return !pTechno || pTechno->CloakState == CloakState::Cloaked || !includeCloaking || pTechno->CloakState == CloakState::Cloaking;
}

bool IsDeadOrInvisibleOrCloaked(TechnoClass* pTechno, bool includeCloaking)
{
	return IsDeadOrInvisible(pTechno) || IsCloaked(pTechno, includeCloaking);
}

double GetROFMulti(TechnoClass* pTechno)
{
	if (IsDead(pTechno))
	{
		return 1;
	}
	bool rof = false;
	if (pTechno->Veterancy.IsElite())
	{
		rof = pTechno->GetTechnoType()->VeteranAbilities.ROF || pTechno->GetTechnoType()->EliteAbilities.ROF;
	}
	else if (pTechno->Veterancy.IsVeteran())
	{
		rof = pTechno->GetTechnoType()->VeteranAbilities.ROF;
	}
	return (!rof ? 1.0 : RulesClass::Instance->VeteranROF) * pTechno->FirepowerMultiplier * ((!pTechno->Owner || !pTechno->Owner->Type) ? 1.0 : pTechno->Owner->Type->ROFMult);
}

double GetDamageMulti(TechnoClass* pTechno)
{
	if (IsDead(pTechno))
	{
		return 1;
	}
	bool firepower = false;
	if (pTechno->Veterancy.IsElite())
	{
		firepower = pTechno->GetTechnoType()->VeteranAbilities.FIREPOWER || pTechno->GetTechnoType()->EliteAbilities.FIREPOWER;
	}
	else if (pTechno->Veterancy.IsVeteran())
	{
		firepower = pTechno->GetTechnoType()->VeteranAbilities.FIREPOWER;
	}
	return (!firepower ? 1.0 : RulesClass::Instance->VeteranCombat) * pTechno->FirepowerMultiplier * ((!pTechno->Owner || !pTechno->Owner->Type) ? 1.0 : pTechno->Owner->Type->FirepowerMult);
}

void SetExtraSparkleAnim(TechnoClass* pTechno, AnimClass*& pAnim)
{
	if (!IsDeadOrInvisible(pTechno) && pAnim)
	{
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
		{
			status->SetExtraSparkleAnim(pAnim);
		}
	}
}

void ActiveRGBMode(TechnoClass* pTechno)
{
	if (!IsDeadOrInvisible(pTechno))
	{
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
		{
			status->PaintballState.RGBIsPower();
		}
	}
}

bool CanBeBase(TechnoClass* pTechno, bool eligibileForAllyBuilding, int houseIndex, int minX, int maxX, int minY, int maxY)
{
	// 检查位置在范围内
	CoordStruct location = pTechno->GetCoords();
	CellStruct cellPos = CellClass::Coord2Cell(location);
	if (cellPos.X >= minX && cellPos.X <= maxX && cellPos.Y >= minY && cellPos.Y <= maxY)
	{
		// 判断所属
		HouseClass* pTargetHouse = pTechno->Owner;
		return pTargetHouse->ArrayIndex == houseIndex || (eligibileForAllyBuilding && pTargetHouse->IsAlliedWith(houseIndex));
	}
	return false;
}

bool CanAttack(TechnoClass* pAttacker, AbstractClass* pTarget, bool isPassiveAcquire)
{
	bool canAttack = false;
	int weaponIdx = pAttacker->SelectWeapon(pTarget);
	WeaponStruct* pWeaponStruct = pAttacker->GetWeapon(weaponIdx);
	WeaponTypeClass* pWeapon = nullptr;
	if (pWeaponStruct && (pWeapon = pWeaponStruct->WeaponType) != nullptr)
	{
		double versus = 1;
		bool forceFire = true;
		bool retaliate = true;
		bool passiveAcquire = true;
		TechnoClass* pTargetTechno = nullptr;
		if (CastToTechno(pTarget, pTargetTechno))
		{
			// 检查护甲
			versus = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWeapon->Warhead)->GetVersus(pTargetTechno->GetTechnoType()->Armor, forceFire, retaliate, passiveAcquire);
		}

		if (isPassiveAcquire)
		{
			// 是否可以主动攻击
			canAttack = versus > 0.2 || passiveAcquire;
		}
		else
		{
			canAttack = versus != 0.0;
		}
		// 检查是否可以攻击
		if (canAttack)
		{
			FireError fireError = pAttacker->GetFireError(pTarget, weaponIdx, true);
			switch (fireError)
			{
			case FireError::ILLEGAL:
			case FireError::CANT:
				canAttack = false;
				break;
			}
		}
	}
	else
	{
		// 没有可以用的武器
		canAttack = false;
	}
	return canAttack;
}
#pragma endregion

#pragma endregion BulletClass

BulletType WhatAmI(BulletClass* pBullet)
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

bool IsDead(BulletClass* pBullet, BulletStatus*& status)
{
	status = nullptr;
	return !pBullet || !pBullet->Type || pBullet->Health == 0 || !pBullet->IsAlive || !TryGetStatus<BulletExt>(pBullet, status) || !status || status->life.IsDetonate;
}

bool IsDead(BulletClass* pBullet)
{
	BulletStatus* status = nullptr;
	return IsDead(pBullet, status);
}

bool IsDeadOrInvisible(BulletClass* pBullet, BulletStatus*& status)
{
	return IsDead(pBullet, status) || pBullet->InLimbo;
}

bool IsDeadOrInvisible(BulletClass* pBullet)
{
	BulletStatus* status = nullptr;
	return IsDeadOrInvisible(pBullet, status);
}

void SetSourceHouse(BulletClass* pBullet, HouseClass* pHouse)
{
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		status->pSourceHouse = pHouse;
	}
}

HouseClass* GetHouse(BulletClass* pBullet)
{
	if (pBullet->Owner)
	{
		return pBullet->Owner->Owner;
	}
	return GetSourceHouse(pBullet);
}

HouseClass* GetSourceHouse(BulletClass* pBullet)
{
	if (BulletStatus* status = GetStatus<BulletExt, BulletStatus>(pBullet))
	{
		return status->pSourceHouse;
	}
	if (pBullet->Owner)
	{
		return pBullet->Owner->Owner;
	}
	return nullptr;
}

DirStruct Facing(BulletClass* pBullet, CoordStruct location)
{
	CoordStruct source = location;
	if (location == CoordStruct::Empty)
	{
		source = pBullet->GetCoords();
	}
	CoordStruct forward = source + ToCoordStruct(pBullet->Velocity);
	return Point2Dir(source, forward);
}

#pragma endregion

#pragma region HouseClass
bool IsCivilian(HouseClass* pHouse)
{
	return !pHouse || pHouse->Defeated || !pHouse->Type
		|| pHouse->Type->MultiplayPassive;
	// || HouseClass.NEUTRAL == pHouse->Type->Base.ID // 自然也算平民吗？
	// || HouseClass.CIVILIAN == pHouse->Type->Base.ID
	// || HouseClass.SPECIAL == pHouse->Type->Base.ID; // 被狙掉驾驶员的阵营是Special
}

Relation GetRelation(HouseClass* pHosue, HouseClass* pTargetHouse)
{
	if (pHosue == pTargetHouse)
	{
		return Relation::OWNER;
	}
	if (pHosue->IsAlliedWith(pTargetHouse))
	{
		return Relation::ALLIES;
	}
	return Relation::ENEMIES;
}

Relation GetRelationWithPlayer(HouseClass* pHouse)
{
	return GetRelation(pHouse, HouseClass::CurrentPlayer);
}

bool AutoRepel(HouseClass* pHouse)
{
	if (pHouse->CurrentPlayer)
	{
		return CombatDamage::Data()->PlayerAutoRepel;
	}
	return CombatDamage::Data()->AutoRepel;
}

bool CanAffectHouse(HouseClass* pHouse, HouseClass* pTargetHouse, bool owner, bool allied, bool enemies, bool civilian)
{
	if (pHouse && pTargetHouse
		&& ((IsCivilian(pTargetHouse) && !civilian)
			|| (pTargetHouse == pHouse ? !owner : (pTargetHouse->IsAlliedWith(pHouse) ? !allied : !enemies))
			))
	{
		return false;
	}
	return true;
}
#pragma endregion
