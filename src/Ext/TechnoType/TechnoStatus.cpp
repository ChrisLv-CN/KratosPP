#include "TechnoStatus.h"

#include <Ext/Common/FireSuperManager.h>
#include <Ext/Common/PrintTextManager.h>

#include <Ext/ObjectType/AttachEffect.h>

#include "AutoFireAreaWeapon.h"
#include "BaseNormal.h"
#include "CrawlingFLH.h"
#include "DamageText.h"
#include "HealthText.h"
#include "JumpjetFacing.h"
#include "MissileHoming.h"
#include "Spawn.h"
#include "TechnoTrail.h"

void TechnoStatus::ExtChanged()
{
	_absType = AbstractType::None;
	_locoType = LocoType::None;

	_destroyAnimData = nullptr;

	_transformData = nullptr;

	// 重新附加其他的组件
	InitExt();
}

void TechnoStatus::Awake()
{
	// 动态附加其他的组件
	InitExt();
}

void TechnoStatus::Destroy()
{
	((TechnoExt::ExtData*)_extData)->SetExtStatus(nullptr);
}

void TechnoStatus::InitExt()
{
	FindOrAttach<AutoFireAreaWeapon>();
	FindOrAttach<DamageText>();
	FindOrAttach<HealthText>();
	FindOrAttach<Spawn>();
	if (!IsBuilding())
	{
		// 初始化尾巴
		FindOrAttach<TechnoTrail>();
		FindOrAttach<BaseNormal>();
		if (IsInfantry())
		{
			FindOrAttach<CrawlingFLH>();
		}
		if (IsJumpjet())
		{
			FindOrAttach<JumpjetFacing>();
		}
		if (IsRocket())
		{
			FindOrAttach<MissileHoming>();
		}
	}
}

AttachEffect* TechnoStatus::AEManager()
{
	AttachEffect* aeManager = nullptr;
	if (_parent)
	{
		aeManager = _parent->GetComponent<AttachEffect>();
	}
	return aeManager;
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
			OnUpdate_DeployToTransform();
		}
		OnUpdate_Transform();

		OnUpdate_AntiBullet();
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
}

void TechnoStatus::OnReceiveDamageEnd(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	if (damageState == DamageState::NowDead)
	{
		// 被打死时读取弹头设置
		OnReceiveDamageEnd_DestroyAnim(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
	}
	OnReceiveDamageEnd_BlackHole(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
	OnReceiveDamageEnd_GiftBox(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
}

void TechnoStatus::OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse) {};

void TechnoStatus::OnReceiveDamageDestroy()
{
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
