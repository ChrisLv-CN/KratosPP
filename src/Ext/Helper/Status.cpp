#include "Status.h"

#include <FootClass.h>
#include <Interfaces.h>
#include <DriveLocomotionClass.h>
#include <MechLocomotionClass.h>
#include <ShipLocomotionClass.h>
#include <WalkLocomotionClass.h>
#include <JumpjetLocomotionClass.h>
#include <CellClass.h>
#include <MapClass.h>


#include "CastEx.h"
#include "Scripts.h"

#include <Extension/WarheadTypeExt.h>

#include <Ext/Common/CommonStatus.h>
#include <Ext/AnimType/AnimStatus.h>
#include <Ext/BulletType/BulletStatus.h>
#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/ObjectType/AttachEffect.h>


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

void ShowAnim(AnimClass* pAnim, Relation visibility)
{
	AnimStatus* status = nullptr;
	if (TryGetStatus<AnimExt, AnimStatus>(pAnim, status))
	{
		status->UpdateVisibility(visibility);
	}
	else
	{
		pAnim->Invisible = false;
	}
}

void HiddenAnim(AnimClass* pAnim)
{
	pAnim->Invisible = true;
}
#pragma endregion

#pragma region ObjectClass
bool IsDead(ObjectClass* pObject)
{
	// if pObject is a bullet, the Health maybe < 0
	return !pObject || pObject->Health == 0 || !pObject->IsAlive || !pObject->GetType();
}
bool IsDeadOrInvisible(ObjectClass* pObject)
{
	return IsDead(pObject) || pObject->InLimbo;
}
#pragma endregion

#pragma endregion TechnoClass
LocoType GetLocoType(TechnoClass* pTechno)
{
	if (pTechno && pTechno->GetTechnoType())
	{
		GUID locoId = pTechno->GetTechnoType()->Locomotor;
		if (locoId == LocomotionClass::CLSIDs::Drive)
		{
			return LocoType::Drive;
		}
		else if (locoId == LocomotionClass::CLSIDs::Hover)
		{
			return LocoType::Hover;
		}
		else if (locoId == LocomotionClass::CLSIDs::Tunnel)
		{
			return LocoType::Tunnel;
		}
		else if (locoId == LocomotionClass::CLSIDs::Walk)
		{
			return LocoType::Walk;
		}
		else if (locoId == LocomotionClass::CLSIDs::Droppod)
		{
			return LocoType::Droppod;
		}
		else if (locoId == LocomotionClass::CLSIDs::Fly)
		{
			return LocoType::Fly;
		}
		else if (locoId == LocomotionClass::CLSIDs::Teleport)
		{
			return LocoType::Teleport;
		}
		else if (locoId == LocomotionClass::CLSIDs::Mech)
		{
			return LocoType::Mech;
		}
		else if (locoId == LocomotionClass::CLSIDs::Ship)
		{
			return LocoType::Ship;
		}
		else if (locoId == LocomotionClass::CLSIDs::Jumpjet)
		{
			return LocoType::Jumpjet;
		}
		else if (locoId == LocomotionClass::CLSIDs::Rocket)
		{
			return LocoType::Rocket;
		}
	}
	return LocoType::None;
}

bool IsDead(TechnoClass* pTechno)
{
	return !pTechno || pTechno->Health <= 0 || !pTechno->IsAlive || pTechno->IsCrashing || pTechno->IsSinking || !pTechno->GetType();
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

bool IsImmune(TechnoClass* pTechno, bool checkStand)
{
	bool immune = pTechno->GetTechnoType()->Immune;
	if (!immune && checkStand)
	{
		TechnoStatus* status = nullptr;
		if (TryGetStatus<TechnoExt>(pTechno, status) && status->AmIStand())
		{
			immune = status->MyStandData.Immune;
		}
	}
	if (!immune)
	{
		// IsForceShilded不能用于判断是否整处于护盾状态，因为启用一次之后永久为1
		immune = pTechno->IsIronCurtained();
	}
	return immune;
}

bool AmIStand(TechnoClass* pTechno)
{
	if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno))
	{
		return status->AmIStand();
	}
	return false;
}

double GetROFMulti(TechnoClass* pTechno)
{
	if (IsDead(pTechno))
	{
		return 1;
	}
	bool rofAbility = false;
	if (pTechno->Veterancy.IsElite())
	{
		rofAbility = pTechno->GetTechnoType()->VeteranAbilities.ROF || pTechno->GetTechnoType()->EliteAbilities.ROF;
	}
	else if (pTechno->Veterancy.IsVeteran())
	{
		rofAbility = pTechno->GetTechnoType()->VeteranAbilities.ROF;
	}
	return (!rofAbility ? 1.0 : RulesClass::Instance->VeteranROF) * ((!pTechno->Owner || !pTechno->Owner->Type) ? 1.0 : pTechno->Owner->Type->ROFMult);
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

int GetRangePlus(TechnoClass* pTechno, bool targetInAir)
{
	int range = 0;
	if (!IsDead(pTechno))
	{
		if (targetInAir)
		{
			range += pTechno->GetTechnoType()->AirRangeBonus;
		}
		if (pTechno->CanOccupyFire())
		{
			range = (RulesClass::Instance->OccupyWeaponRange + pTechno->GetOccupyRangeBonus()) * Unsorted::LeptonsPerCell;
		}
		if (pTechno->BunkerLinkedItem && pTechno->WhatAmI() != AbstractType::Building)
		{
			range += RulesClass::Instance->BunkerWeaponRangeBonus * Unsorted::LeptonsPerCell;
		}
		if (pTechno->InOpenToppedTransport)
		{
			range += RulesClass::Instance->OpenToppedRangeBonus * Unsorted::LeptonsPerCell;
		}
		if (AttachEffect* aem = GetAEManager<TechnoExt>(pTechno))
		{
			CrateBuffData data = aem->CountAttachStatusMultiplier();
			range += (int)(data.RangeCell * Unsorted::LeptonsPerCell);
			range *= data.RangeMultiplier;
		}
	}
	return range;
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
			status->Paintball->RGBIsPower();
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

bool IsOnMark(TechnoClass* pTechno, FilterData data)
{
	bool hasWhiteList = !data.OnlyAffectMarks.empty();
	bool hasBlackList = !data.NotAffectMarks.empty();
	bool mark = !hasWhiteList && !hasBlackList;
	if (!mark)
	{
		if (AttachEffect* aem = GetAEManager<TechnoExt>(pTechno))
		{
			std::vector<std::string> marks{};
			aem->GetMarks(marks);
			mark = data.OnMark(marks);
		}
	}
	return mark;
}

bool CanAttack(BulletClass* pBullet, TechnoClass* pTarget, bool isPassiveAcquire)
{
	bool canAttack = false;
	WarheadTypeClass* pWH = pBullet->WH;
	if (pWH)
	{
		// 判断护甲
		bool forceFire = true;
		bool retaliate = true;
		bool passiveAcquire = true;
		double versus = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH)->GetVersus(pTarget->GetTechnoType()->Armor, forceFire, retaliate, passiveAcquire);
		if (isPassiveAcquire)
		{
			// 是否可以主动攻击
			canAttack = versus > 0.2 || passiveAcquire;
		}
		else
		{
			canAttack = versus != 0.0;
		}
		// 能不能对空
		if (canAttack && pTarget->IsInAir())
		{
			canAttack = pBullet->Type->AA;
		}
	}
	return canAttack;
}

bool CanAttack(TechnoClass* pAttacker, AbstractClass* pTarget, bool inRange, int weaponIdx, bool isPassiveAcquire)
{
	bool canAttack = false;
	if (weaponIdx < 0)
	{
		weaponIdx = pAttacker->SelectWeapon(pTarget);
	}
	WeaponStruct* pWeaponStruct = pAttacker->GetWeapon(weaponIdx);
	WeaponTypeClass* pWeapon = nullptr;
	if (pWeaponStruct && (pWeapon = pWeaponStruct->WeaponType) != nullptr && (!inRange || pAttacker->IsCloseEnough(pTarget, weaponIdx)))
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


int GetRealDamage(Armor armor, int damage, WarheadTypeClass* pWH, bool ignoreArmor, int distance)
{
	int realDamage = damage;
	if (!ignoreArmor)
	{
		// 计算实际伤害
		if (realDamage > 0)
		{
			realDamage = MapClass::GetTotalDamage(damage, pWH, armor, distance);
		}
		else
		{
			realDamage = -MapClass::GetTotalDamage(-damage, pWH, armor, distance);
		}
	}
	return realDamage;
}

bool CanDamageMe(TechnoClass* pTechno, int damage, int distanceFromEpicenter, WarheadTypeClass* pWH, int& realDamage, bool effectsRequireDamage)
{
	// 计算实际伤害
	realDamage = GetRealDamage(pTechno->GetType()->Armor, damage, pWH, false, distanceFromEpicenter);
	WarheadTypeExt::TypeData* data = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
	if (damage == 0)
	{
		return data->AllowZeroDamage;
	}
	else
	{
		if (data->EffectsRequireVerses)
		{
			// 必须要可以造成伤害
			if (MapClass::GetTotalDamage(RulesClass::Instance->MaxDamage, pWH, pTechno->GetType()->Armor, 0) == 0)
			{
				// 弹头无法对该类型护甲造成伤害
				return false;
			}
			// 伤害非零，当EffectsRequireDamage=yes时，必须至少造成1点实际伤害
			if (effectsRequireDamage || data->EffectsRequireDamage)
			{
				return realDamage != 0;
			}
		}
	}
	return true;
}

bool CanAffectMe(TechnoClass* pTechno, HouseClass* pAttackingHouse, WarheadTypeClass* pWH)
{
	HouseClass* pHouse = pTechno->Owner;
	WarheadTypeExt::TypeData* whData = nullptr;
	return TryGetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH, whData)
		&& CanAffectHouse(pHouse, pAttackingHouse, whData->AffectsOwner, whData->AffectsAllies, whData->AffectsEnemies);
}

void ClearAllTarget(TechnoClass* pAttacker)
{
	if (pAttacker)
	{
		// 自身
		pAttacker->Target = nullptr;
		pAttacker->SetTarget(nullptr);
		pAttacker->QueueMission(Mission::Stop, true);
		// 子机管理器
		if (pAttacker->SpawnManager)
		{
			pAttacker->SpawnManager->Destination = nullptr;
			pAttacker->SpawnManager->Target = nullptr;
			pAttacker->SpawnManager->SetTarget(nullptr);
		}
		// 超时空传送
		if (pAttacker->TemporalImUsing)
		{
			pAttacker->TemporalImUsing->LetGo();
		}
	}
}

void ForceStopMoving(FootClass* pFoot)
{
	// 清除移动目的地
	pFoot->StopMoving();
	pFoot->SetDestination(nullptr, true);
	pFoot->SetFocus(nullptr);
	pFoot->Destination = nullptr;
	pFoot->LastDestination = nullptr;
	pFoot->Focus = nullptr;
	// 清除寻路目的地
	ILocomotion* pLoco = pFoot->Locomotor.get();
	pLoco->Mark_All_Occupation_Bits((int)PlacementType::Remove); // 清除HeadTo的占领
	ForceStopMoving(pLoco);
}

void ForceStopMoving(ILocomotion* pLoco)
{
	pLoco->Stop_Moving();
	pLoco->Mark_All_Occupation_Bits((int)PlacementType::Remove);
	if (DriveLocomotionClass* dLoco = dynamic_cast<DriveLocomotionClass*>(pLoco))
	{
		dLoco->Destination = CoordStruct::Empty;
		dLoco->HeadToCoord = CoordStruct::Empty;
		dLoco->IsDriving = false;
	}
	else if (ShipLocomotionClass* sLoco = dynamic_cast<ShipLocomotionClass*>(pLoco))
	{
		sLoco->Destination = CoordStruct::Empty;
		sLoco->HeadToCoord = CoordStruct::Empty;
		sLoco->IsDriving = false;
	}
	else if (WalkLocomotionClass* wLoco = dynamic_cast<WalkLocomotionClass*>(pLoco))
	{
		wLoco->Destination = CoordStruct::Empty;
		wLoco->HeadToCoord = CoordStruct::Empty;
		wLoco->IsMoving = false;
		wLoco->IsReallyMoving = false;
	}
	else if (MechLocomotionClass* mLoco = dynamic_cast<MechLocomotionClass*>(pLoco))
	{
		mLoco->Destination = CoordStruct::Empty;
		mLoco->HeadToCoord = CoordStruct::Empty;
		mLoco->IsMoving = false;
	}
	else if (JumpjetLocomotionClass* jLoco = dynamic_cast<JumpjetLocomotionClass*>(pLoco))
	{
		jLoco->DestinationCoords = CoordStruct::Empty;
		jLoco->IsMoving = false;
	}
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

bool IsDead(BulletClass* pBullet, BulletStatus* status)
{
	if (!status)
	{
		TryGetStatus<BulletExt>(pBullet, status);
	}
	return !pBullet || !pBullet->Type || pBullet->Health == 0 || !pBullet->IsAlive || !status || status->life.IsDetonate;
}

bool IsDead(BulletClass* pBullet)
{
	return IsDead(pBullet, nullptr);
}

bool IsDeadOrInvisible(BulletClass* pBullet, BulletStatus* status)
{
	return IsDead(pBullet, status) || pBullet->InLimbo;
}

bool IsDeadOrInvisible(BulletClass* pBullet)
{
	return IsDeadOrInvisible(pBullet, nullptr);
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
	// || HouseClass.NEUTRAL == pHouse->Type->ID // 自然也算平民吗？
	// || HouseClass.CIVILIAN == pHouse->Type->ID
	// || HouseClass.SPECIAL == pHouse->Type->ID; // 被狙掉驾驶员的阵营是Special
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

#pragma region WarheadTypeClass
AnimClass* PlayWarheadAnim(WarheadTypeClass* pWH, CoordStruct location, int damage, LandType landType)
{
	AnimClass* pAnim = nullptr;
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		landType = pCell->LandType;
	}
	if (AnimTypeClass* pAnimType = MapClass::Instance->SelectDamageAnimation(damage, pWH, landType, location))
	{
		pAnim = GameCreate<AnimClass>(pAnimType, location);
	}
	return pAnim;
}
#pragma endregion
