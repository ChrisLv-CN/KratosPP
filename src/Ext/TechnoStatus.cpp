#include "TechnoStatus.h"

#include <Ext/FireSuperManager.h>

bool TechnoStatus::OnAwake()
{
	VoxelShadowScaleInAir = INI::GetSection(INI::Rules, INI::SectionAudioVisual)->Get("VoxelShadowScaleInAir", VoxelShadowScaleInAir);
	return true;
}

void TechnoStatus::OnTransform(TypeChangeEventArgs* args)
{
	TechnoClass* pTarget = args->pTechno;
	if (pTarget && pTarget == pTechno)
	{
		_absType = AbstractType::None;
		_locoType = LocoType::None;
		_crawlingFLHData = nullptr;
		_transformData = nullptr;
	}
}

void TechnoStatus::Destroy()
{
	((TechnoExt::ExtData*)extData)->SetExtStatus(nullptr);
}

AbstractType TechnoStatus::GetAbsType()
{
	if (_absType == AbstractType::None)
	{
		_absType = pTechno->What_Am_I();
	}
	return _absType;
}


bool TechnoStatus::IsBuilding()
{
	return GetAbsType() == AbstractType::Building;
}
bool TechnoStatus::IsInfantry()
{
	return GetAbsType() == AbstractType::Infantry;
}
bool TechnoStatus::IsUnit()
{
	return GetAbsType() == AbstractType::Unit;
}
bool TechnoStatus::IsAircraft()
{
	return GetAbsType() == AbstractType::Aircraft;
}

LocoType TechnoStatus::GetLocoType()
{
	if (!IsBuilding())
	{
		if (_locoType == LocoType::None)
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
	}
	return _locoType;
}

bool TechnoStatus::IsFly()
{
	return GetLocoType() == LocoType::Fly;
}
bool TechnoStatus::IsJumpjet()
{
	return GetLocoType() == LocoType::Jumpjet;
}
bool TechnoStatus::IsShip()
{
	return GetLocoType() == LocoType::Ship;
}

bool TechnoStatus::AmIStand()
{
	return false;
}

void TechnoStatus::OnPut(CoordStruct* pLocation, DirType dirType)
{
	if (!_initStateFlag)
	{
		_initStateFlag = true;
		InitState();
	}
}

void TechnoStatus::InitState()
{
	InitState_Paintball();
}

void TechnoStatus::OnUpdate()
{
	if (!IsDead(pTechno))
	{
		switch (pTechno->CurrentMission)
		{
		case Mission::Move:
		case Mission::AttackMove:
			// 上一次任务不是这两个说明是起步
			if (Mission::Move != _lastMission && Mission::AttackMove != _lastMission)
			{
				drivingState = DrivingState::Start;
			}
			else
			{
				drivingState = DrivingState::Moving;
			}
			break;
		default:
			// 上一次任务如果是Move或者AttackMove说明是刹车
			if (Mission::Move == _lastMission || Mission::AttackMove == _lastMission)
			{
				drivingState = DrivingState::Stop;
			}
			else
			{
				drivingState = DrivingState::Stand;
			}
			break;
		}
		if (!IsBuilding())
		{
			FootClass* pFoot = static_cast<FootClass*>(pTechno);
			_isMoving = pFoot->GetCurrentSpeed() > 0 && pFoot->Locomotor.get()->Is_Moving();
		}
		OnUpdate_CrawlingFLH();
		OnUpdate_DamageText();
		OnUpdate_DeployToTransform();
		OnUpdate_GiftBox();
		OnUpdate_Paintball();
	}
}

void TechnoStatus::OnUpdateEnd()
{
	if (!IsDeadOrInvisible(pTechno))
	{
		this->_lastMission = pTechno->CurrentMission;
	}
}

void TechnoStatus::OnTemporalUpdate(TemporalClass* pTemporal)
{
	// for Stand
	if (pTemporal && pTemporal->Owner)
	{
		TechnoClass* pAttacker = pTemporal->Owner;
		int weaponIdx = pAttacker->SelectWeapon(pTechno);
		if (weaponIdx < 0 || !pAttacker->IsCloseEnough(pTechno, weaponIdx))
		{
			pTemporal->LetGo();
		}
	}
}

void TechnoStatus::OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	if (damageState == DamageState::NowDead)
	{
		// 被打死时读取弹头设置
		OnReceiveDamageEnd_DestroyAnim(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
	}
	OnReceiveDamageEnd_BlackHole(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
	OnReceiveDamageEnd_DamageText(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
	OnReceiveDamageEnd_GiftBox(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
}

void TechnoStatus::OnReceiveDamageEnd_DestroyAnim(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) {};
void TechnoStatus::OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) {};

void TechnoStatus::OnReceiveDamageDestroy()
{
	OnReceiveDamageDestroy_GiftBox();
}

void TechnoStatus::OnFire(AbstractClass* pTarget, int weaponIdx)
{
	OnFire_FireSuper(pTarget, weaponIdx);
}
