#include "TechnoStatus.h"

#include <Ext/Common/CommonStatus.h>
#include <Ext/Common/FireSuperManager.h>
#include <Ext/Common/PrintTextManager.h>

#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/Scripts.h>

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

AttachEffect* TechnoStatus::AEManager()
{
	AttachEffect* aeManager = nullptr;
	if (_parent)
	{
		aeManager = _parent->GetComponent<AttachEffect>();
	}
	return aeManager;
}

void TechnoStatus::InitState()
{
	pSourceType = pTechno->GetTechnoType();
	pTargetType = pSourceType;

	_deactivateDimEMP = AudioVisual::Data()->DeactivateDimEMP;
	_deactivateDimPowered = AudioVisual::Data()->DeactivateDimPowered;

	if (INI::GetSection(INI::Rules, pTechno->GetTechnoType()->ID)->Get("VirtualUnit", false))
	{
		VirtualUnit = true;
	}
	AttachState();
}

void TechnoStatus::InitExt()
{
	Tag.append("[").append(pTechno->GetTechnoType()->ID).append("]")
		.append(WString2String(pTechno->GetTechnoType()->UIName));

	_gameObject->FindOrAttach<AutoFireAreaWeapon>();
	_gameObject->FindOrAttach<DamageText>();
	_gameObject->FindOrAttach<HealthText>();
	_gameObject->FindOrAttach<Spawn>();
	if (!IsBuilding())
	{
		// 初始化尾巴
		_gameObject->FindOrAttach<TechnoTrail>();
		_gameObject->FindOrAttach<BaseNormal>();
		if (IsInfantry())
		{
			_gameObject->FindOrAttach<CrawlingFLH>();
		}
		if (IsJumpjet())
		{
			_gameObject->FindOrAttach<JumpjetFacing>();
		}
		if (IsRocket())
		{
			_gameObject->FindOrAttach<MissileHoming>();
		}
	}
}

void TechnoStatus::Awake()
{
	// 初始化状态机
	InitState();
	// 动态附加其他的组件
	InitExt();
}

void TechnoStatus::Destroy()
{
	((TechnoExt::ExtData*)_extData)->SetExtStatus(nullptr);
}

void TechnoStatus::ExtChanged()
{
	_absType = AbstractType::None;
	_locoType = LocoType::None;

	// 重新附加其他的组件
	InitExt();
}

void TechnoStatus::OnPut(CoordStruct* pCoord, DirType dirType)
{
	OnPut_Stand(pCoord, dirType);
}

void TechnoStatus::OnRemove()
{
	OnRemove_Stand();
}

void TechnoStatus::OnUpdate()
{
	OnUpdate_DestroySelf();
	if (!_isDead && !IsDead(pTechno))
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
		OnUpdate_Deselect();
		OnUpdate_GiftBox();
		OnUpdate_Paintball();
	}
}

void TechnoStatus::OnUpdateEnd()
{
	if (!_isDead && !IsDeadOrInvisible(pTechno))
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

void TechnoStatus::OnReceiveDamage(args_ReceiveDamage* args)
{
	// 弹头效果
	if (!IsDeadOrInvisible(pTechno))
	{
		if (WarheadTypeExt::TypeData* whTypeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(args->WH))
		{
			int realDamage = *args->Damage;
			if (CanAffectHouse(pTechno->Owner, args->SourceHouse, whTypeData->AffectsOwner, whTypeData->AffectsAllies, whTypeData->AffectsEnemies)
				&& CanDamageMe(pTechno, *args->Damage, args->DistanceToEpicenter, args->WH, realDamage, whTypeData->EffectsRequireDamage))
			{
				// 清理目标弹头
				if (whTypeData->ClearTarget && pTechno->Target)
				{
					ClearAllTarget(pTechno);
				}
				// 清理伪装弹头
				if (whTypeData->ClearDisguise && pTechno->IsDisguised())
				{
					pTechno->Disguised = false;
				}
				// 欠揍弹头
				TechnoClass* pAttacker = nullptr;
				if (whTypeData->Lueluelue && CastToTechno(args->Attacker, pAttacker) && CanAttack(pTechno, pAttacker))
				{
					pTechno->SetTarget(pAttacker);
					pTechno->ForceMission(Mission::Attack);
				}
				// 强制任务弹头
				Mission forceMission = whTypeData->ForceMission;
				if (forceMission != Mission::None)
				{
					pTechno->ForceMission(forceMission);
				}
				// 经验修改弹头
				if ((whTypeData->ExpCost != 0 || whTypeData->ExpLevel != ExpLevel::None) && pTechno->GetTechnoType()->Trainable)
				{
					switch (whTypeData->ExpLevel)
					{
					case ExpLevel::Rookie:
						pTechno->Veterancy.SetRookie();
						break;
					case ExpLevel::Veteran:
						pTechno->Veterancy.SetVeteran();
						break;
					case ExpLevel::Elite:
						pTechno->Veterancy.SetElite();
						break;
					default:
						int cost = pTechno->GetTechnoType()->GetActualCost(pTechno->Owner);
						pTechno->Veterancy.Add(cost, whTypeData->ExpCost);
						break;
					}
				}
			}
		}
	}
	OnReceiveDamage_Stand(args);
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
	OnReceiveDamageEnd_Vampire(pRealDamage, pWH, damageState, pAttacker, pAttackingHouse);
}

void TechnoStatus::OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{

};

void TechnoStatus::OnReceiveDamageDestroy()
{
	OnReceiveDamageDestroy_Transform();

	OnReceiveDamageDestroy_GiftBox();
}

void TechnoStatus::OnRegisterDestruction(TechnoClass* pKiller, int cost, bool& skip)
{
	OnRegisterDestruction_Stand(pKiller, cost, skip);
	if (skip)
	{
		return;
	}
}

void TechnoStatus::CanFire(AbstractClass* pTarget, WeaponTypeClass* pWeapon, bool& ceaseFire)
{
	// 目标对特定弹头免疫
	if (!pWeapon || !pWeapon->Warhead)
	{
		return;
	}
	TechnoClass* pTargetTechno = nullptr;
	AttachEffect* aem = nullptr;
	if (CastToTechno(pTarget, pTargetTechno) && TryGetAEManager<TechnoExt>(pTargetTechno, aem))
	{
		ImmuneData immune = aem->GetImmuneData();
		if (immune.CeaseFire(pWeapon->Warhead->ID))
		{
			ceaseFire = true;
		}
	}
}

void TechnoStatus::OnSelect(bool& selectable)
{
	selectable = OnSelect_VirtualUnit();
	if (!selectable)
	{
		Break();
		return;
	}
	selectable = OnSelect_Deselect();
	if (!selectable)
	{
		Break();
		return;
	}
}
