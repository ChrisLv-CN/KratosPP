#include "TechnoStatus.h"
#include "TechnoTrail.h"

#include <Ext/Common/FireSuperManager.h>
#include <Ext/Common/PrintTextManager.h>

void TechnoStatus::Awake()
{
	// 动态附加其他的组件
	ResetTrails();
}

void TechnoStatus::Destroy()
{
	((TechnoExt::ExtData*)_extData)->SetExtStatus(nullptr);
}

void TechnoStatus::ResetTrails()
{
	if (!IsBuilding())
	{
		TechnoTrail* trail = _gameObject->FindOrAttach<TechnoTrail>();
		if (trail)
		{
			trail->SetupTrails();
		}
	}
}

void TechnoStatus::OnTransform()
{
	_absType = AbstractType::None;
	_locoType = LocoType::None;

	_autoAreaData = nullptr;

	_destroyAnimData = nullptr;

	_crawlingFLHData = nullptr;
	_transformData = nullptr;

	_spawnData = nullptr;
	_homingData = nullptr;

	_jjFacingData = nullptr;

	ResetBaseNormal();
	// 通知其他脚本
	ResetTrails();
}

bool TechnoStatus::AmIStand()
{
	return false;
}

void TechnoStatus::OnPut(CoordStruct* pLocation, DirType dirType)
{
	if (!IsHoming)
	{
		IsHoming = GetHomingData()->Homing;
	}
	if (!_initStateFlag)
	{
		_initStateFlag = true;
		InitState();
		OnPut_AutoArea(pLocation, dirType);
		OnPut_BaseNormarl(pLocation, dirType);
	}
}

void TechnoStatus::InitState()
{
	InitState_AntiBullet();
	InitState_CrateBuff();
	InitState_DestroyAnim();
	InitState_DestroySelf();
	InitState_FireSuper();
	InitState_GiftBox();
	InitState_Paintball();
	InitState_Transform();
}

void TechnoStatus::OnUpdate()
{
	OnUpdate_DestroySelf();
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
			OnUpdate_BaseNormal();
			OnUpdate_CrawlingFLH();
			OnUpdate_DeployToTransform();
			OnUpdate_MissileHoming();
			OnUpdate_JJFacing();
		}
		OnUpdate_Transform();

		OnUpdate_AutoArea();
		OnUpdate_AntiBullet();
		OnUpdate_DamageText();
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

void TechnoStatus::OnWarpUpdate()
{
	// for Stand
	OnWarpUpdate_DestroySelf_Stand();
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

void TechnoStatus::OnRemove()
{
	OnRemove_BaseNormarl();
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

void TechnoStatus::OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) {};

void TechnoStatus::OnReceiveDamageDestroy()
{
	OnReceiveDamageDestroy_BaseNormarl();
	OnReceiveDamageDestroy_Transform();

	OnReceiveDamageDestroy_GiftBox();
}

void TechnoStatus::OnFire(AbstractClass* pTarget, int weaponIdx)
{
	OnFire_FireSuper(pTarget, weaponIdx);
}

void TechnoStatus::OnSelect(bool& selectable)
{
	selectable = OnSelect_VirtualUnit();
	if (!selectable)
	{
		return;
	}
	selectable = OnSelect_Deselect();
	if (!selectable)
	{
		return;
	}
}
