#include "Stand.h"

#include <Ext/Helper/FLH.h>
#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Scripts.h>
#include <Ext/Helper/Status.h>

#include <Ext/TechnoType/TechnoStatus.h>

void Stand::CreateAndPutStand()
{
	CoordStruct location = pObject->GetCoords();
	pStand = CreateAndPutTechno(Data.Type, AE->pSourceHouse, location);
	if (pStand)
	{
		if (pObject->InLimbo)
		{
			pStand->Limbo();
		}
		// 初始化设置
		if (TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pStand))
		{
			status->VirtualUnit = Data.VirtualUnit;
			status->StandData = Data;

			// 设置替身的所有者
			if (pTechno)
			{
				status->pMyMaster = pTechno;
				// TODO 同阵营同步状态机，比如染色
				TechnoStatus* masterStatus = nullptr;
				if (pTechno->Owner == AE->pSourceHouse && TryGetStatus<TechnoExt>(pTechno, masterStatus))
				{

				}
				if (IsAircraft())
				{
					masterIsRocket = pTechno->GetTechnoType()->MissileSpawn;
					masterIsSpawned = masterIsRocket || pTechno->GetTechnoType()->Spawned;
					status->MyMasterIsSpawned = masterIsSpawned;
				}
			}
			else if (pBullet)
			{
				status->pMyMaster = pBullet->Owner;
			}
		}
		pStand->UpdatePlacement(PlacementType::Remove); // Mark(MarkType::Up)
		bool canGuard = AE->pSourceHouse->IsControlledByHuman();
		if (pStand->WhatAmI() == AbstractType::Building)
		{
			standIsBuilding = true;
			canGuard = true;
		}
		else
		{
			dynamic_cast<FootClass*>(pStand)->Locomotor->Lock();
		}
		// only computer units can hunt
		Mission mission = canGuard ? Mission::Guard : Mission::Hunt;
		pStand->QueueMission(mission, false);
		// 移动到指定的位置
		LocationMark locationMark = GetRelativeLocation(pObject, Data.Offset);
		if (!locationMark.IsEmpty())
		{
			SetLocation(locationMark.Location);
			ForceFacing(locationMark.Dir);
		}
		// 攻击来源
		TechnoClass* pSource = AE->pSource;
		if (Data.AttackSource && !IsDeadOrInvisible(pSource) && CanAttack(pStand, pSource))
		{
			pStand->SetTarget(pSource);
		}
	}
}

void Stand::ExplodesOrDisappear(bool peaceful)
{
	bool explodes = !peaceful && (Data.Explodes || notBeHuman || (masterIsRocket && Data.ExplodesWithRocket)) && !pStand->BeingWarpedOut && !pStand->WarpingOut;
	TechnoStatus* standStatus = nullptr;
	if (TryGetStatus<TechnoExt>(pStand, standStatus))
	{
		// Logger.Log($"{Game.CurrentFrame} 阿伟 [{Data.Type}]{pStand} 要死了 explodes = {explodes}");
		standStatus->DestroySelfState.DestroyNow(!explodes);
		// 如果替身处于Limbo状态，OnUpdate不会执行，需要手动触发
		if (masterIsRocket || pStand->InLimbo)
		{
			standStatus->OnUpdate_DestroySelf();
		}
	}
	else
	{
		if (explodes)
		{
			// Logger.Log($"{Game.CurrentFrame} {AEType.Name} 替身 {pStand}[{Type.Type}] 自爆, 没有发现EXT");
			pStand->TakeDamage(pStand->Health + 1, pStand->GetTechnoType()->Crewed);
		}
		else
		{
			// Logger.Log($"{Game.CurrentFrame} {AEType.Name} 替身 {Type.Type} 移除, 没有发现EXT");
			pStand->Limbo();
			// pStand.Ref.Base.UnInit(); // 替身攻击建筑时死亡会导致崩溃，莫名其妙的bug
			pStand->TakeDamage(pStand->Health + 1, false);
		}
	}
	pStand = nullptr;
}

void Stand::UpdateStateBullet()
{
	// synch target
	RemoveStandIllegalTarget();
	AbstractClass* pTarget = pBullet->Target;
	if (pTarget && Data.SameTarget)
	{
		pStand->SetTarget(pTarget);
	}
	if (!pTarget && Data.SameLoseTarget)
	{
		pStand->SetTarget(pTarget);
		if (pStand->SpawnManager)
		{
			pStand->SpawnManager->Destination = pTarget;
		}
	}
	switch (Data.Targeting)
	{
	case StandTargeting::LAND:
		if (pStand->InAir)
		{
			ClearAllTarget(pStand);
		}
		break;
	case StandTargeting::AIR:
		if (!pStand->InAir)
		{
			ClearAllTarget(pStand);
		}
		break;
	}
}

void Stand::UpdateStateTechno(bool masterIsDead)
{
	if (pTechno->IsSinking && Data.RemoveAtSinking)
	{
		ExplodesOrDisappear(true);
		return;
	}
	if (masterIsDead && AE->AEManager->InSelling())
	{
		ExplodesOrDisappear(true);
		return;
	}
	// reset state
	pStand->UpdatePlacement(PlacementType::Remove);

	if (Data.SameHouse)
	{
		pStand->SetOwningHouse(pTechno->Owner);
	}
	// synch state
	pStand->IsSinking = pTechno->IsSinking;
	pStand->unknown_3CA = pTechno->unknown_3CA;
	pStand->InLimbo = pTechno->InLimbo;
	pStand->OnBridge = pTechno->OnBridge;
	if (pStand->Owner == pTechno->Owner)
	{
		// 同阵营限定
		pStand->Cloakable = pTechno->Cloakable;
		pStand->CloakState = pTechno->CloakState;
		pStand->WarpingOut = pTechno->WarpingOut; // 超时空传送冻结
		// 替身不是强制攻击jojo的超时空兵替身
		if (!Data.ForceAttackMaster || !pStand->TemporalImUsing)
		{
			pStand->BeingWarpedOut = pTechno->BeingWarpedOut; // 被超时空兵冻结
		}
		pStand->Deactivated = pTechno->Deactivated; // 遥控坦克

		pStand->IronCurtainTimer = pTechno->IronCurtainTimer;
		pStand->IronTintTimer = pTechno->IronTintTimer;
		// pStand->CloakDelayTimer = pTechno->CloakDelayTimer; // 反复进入隐形
		pStand->IdleActionTimer = pTechno->IdleActionTimer;
		pStand->Berzerk = pTechno->Berzerk;
		pStand->EMPLockRemaining = pTechno->EMPLockRemaining;
		pStand->ShouldLoseTargetNow = pTechno->ShouldLoseTargetNow;

		// synch status
		if (Data.IsVirtualTurret)
		{
			pStand->FirepowerMultiplier = pTechno->FirepowerMultiplier;
			pStand->ArmorMultiplier = pTechno->ArmorMultiplier;
		}

		// synch ammo
		if (Data.SameAmmo)
		{
			pStand->Ammo = pTechno->Ammo;
		}

		// synch Passengers
		if (Data.SamePassengers)
		{
			// Pointer<FootClass> pPassenger = pTechno->Passengers.FirstPassenger;
			// if (!pPassenger.IsNull)
			// {
			//     Pointer<TechnoTypeClass> pType = pPassenger->Base.Type;
			//     Pointer<TechnoClass> pNew = pType->Base.CreateObject(AE.pSourceHouse).Convert<TechnoClass>();
			//     pNew->Base.Put(default, DirType.N);
			//     Logger.Log($"{Game.CurrentFrame} 把jojo的乘客塞进替身里");
			//     pStand->Passengers.AddPassenger(pNew.Convert<FootClass>());
			// }
		}

		// synch Promote
		if (pStand->GetTechnoType()->Trainable)
		{
			if (Data.PromoteFromSpawnOwner && masterIsSpawned && !IsDead(pTechno->SpawnOwner))
			{
				pStand->Veterancy = pTechno->SpawnOwner->Veterancy;
			}
			else if (Data.PromoteFromMaster)
			{
				pStand->Veterancy = pTechno->Veterancy;
			}
		}

		// synch PrimaryFactory
		pStand->IsPrimaryFactory = pTechno->IsPrimaryFactory;
	}

	if (pStand->InLimbo)
	{
		ClearAllTarget(pStand);
		return;
	}

	// Get mission
	Mission mission = pTechno->CurrentMission;

	// check power off and moving
	bool isMoving = mission == Mission::Move || mission == Mission::AttackMove;
	if (IsBuilding())
	{
		if (standIsBuilding && pTechno->Owner == pStand->Owner)
		{
			pStand->Focus = pTechno->Focus;
		}
	}
	else if (!isMoving)
	{
		FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
		isMoving = pFoot->Locomotor->Is_Moving() && pFoot->GetCurrentSpeed() > 0;
	}

	// check fire
	bool powerOff = Data.Powered && AE->AEManager->PowerOff;
	bool canFire = !powerOff && (Data.MobileFire || !isMoving);
	if (canFire)
	{
		// synch mission
		switch (mission)
		{
		case Mission::Guard:
		case Mission::Area_Guard:
			Mission standMission = pStand->CurrentMission;
			if (standMission != Mission::Attack)
			{
				pStand->QueueMission(mission, true);
			}
			break;
		}
	}
	else
	{
		ClearAllTarget(pStand);
		onStopCommand = false;
		pStand->QueueMission(Mission::Sleep, true);
	}

	// synch target
	if (Data.ForceAttackMaster)
	{
		if (!powerOff && !masterIsDead)
		{
			// 替身是超时空兵，被冻住时不能开火，需要特殊处理
			if (pStand->BeingWarpedOut && pStand->TemporalImUsing)
			{
				pStand->BeingWarpedOut = false;
				if (CanAttack(pStand, pTechno))
				{
					// 检查ROF
					if (pStand->ROFTimer.Expired())
					{
						int weaponIdx = pStand->SelectWeapon(pTechno);
						// Logger.Log($"{Game.CurrentFrame} [{Data.Type}]{pStand} 向 JOJO {pTarget} 发射武器");
						pStand->Fire_IgnoreType(pTechno, weaponIdx);
						int rof = 0;
						WeaponStruct* pWeapon = pStand->GetWeapon(weaponIdx);
						if (pWeapon && pWeapon->WeaponType)
						{
							rof = pWeapon->WeaponType->ROF;
						}
						if (rof > 0)
						{
							pStand->ROFTimer.Start(rof);
						}
					}
				}
			}
			else
			{
				if (CanAttack(pStand, pTechno))
				{
					pStand->SetTarget(pTechno);
				}
			}
		}
	}
	else
	{
		if (!onStopCommand)
		{
			// synch Target
			RemoveStandIllegalTarget();
			AbstractClass* pTarget = pTechno->Target;
			if (pTarget)
			{
				if (Data.SameTarget && canFire && CanAttack(pStand, pTarget))
				{
					pStand->SetTarget(pTarget);
				}
			}
			else
			{
				if (Data.SameLoseTarget || !canFire)
				{
					ClearAllTarget(pStand);
				}
			}
		}
		else
		{
			onStopCommand = false;
		}
	}
	switch (Data.Targeting)
	{
	case StandTargeting::LAND:
		if (pStand->InAir)
		{
			ClearAllTarget(pStand);
		}
		break;
	case StandTargeting::AIR:
		if (!pStand->InAir)
		{
			ClearAllTarget(pStand);
		}
		break;
	}

	/* TODO Stand moving anim
			// synch Moving anim
			if (Data.IsTrain || Data.SameMoving)
			{
				FootClass* pFoot = dynamic_cast<FootClass*>(pStand);
				ILocomotion* loco = pFoot->Locomotor;
				Guid locoId = loco.ToLocomotionClass()->GetClassID();
				if (locoId == LocomotionClass.Drive || locoId == LocomotionClass.Walk || locoId == LocomotionClass.Mech)
				{
					if (masterIsMoving)
					{
						if (isMoving)
						{
							if (!Data.IsTrain)
							{
								// 移动前，设置替身的朝向与JOJO相同
								pStand->Facing.set(pMaster->Facing.current());
							}
							// 往前移动，播放移动动画
							if (waklRateTimer.Expired())
							{
								// VXL只需要帧动起来，就会播放动画
								// 但SHP动画，还需要检查Loco.Is_Moving()为true时，才可以播放动画 0x73C69D
								pFoot->WalkedFramesSoFar_idle++;
								waklRateTimer.Start(pFoot->Base.Type->WalkRate);
							}
							// 为SHP素材设置一个总的运动标记
							if (pStand.TryGetStatus(out TechnoStatusScript status))
							{
								status.StandIsMoving = true;
							}
							// DriveLoco.Is_Moving()并不会判断IsDriving
							// ShipLoco.Is_Moving()并不会判断IsDriving
							// HoverLoco.Is_Moving()与前面两个一样，只用位置判断是否在运动
							// 以上几个是通过判断位置来确定是否在运动
							// WalkLoco和MechLoco则只返回IsMoving来判断是否在运动
							if (locoId == LocomotionClass.Walk)
							{
								Pointer<WalkLocomotionClass> pLoco = loco.ToLocomotionClass<WalkLocomotionClass>();
								pLoco->IsReallyMoving = true;
							}
							else if (locoId == LocomotionClass.Mech)
							{
								Pointer<MechLocomotionClass> pLoco = loco.ToLocomotionClass<MechLocomotionClass>();
								pLoco->IsMoving = true;
							}
						}
					}
					else
					{
						if (isMoving)
						{
							// 停止移动
							// 为SHP素材设置一个总的运动标记
							if (pStand.TryGetStatus(out TechnoStatusScript status))
							{
								status.StandIsMoving = false;
							}
							// loco.ForceStopMoving();
							if (locoId == LocomotionClass.Walk)
							{
								Pointer<WalkLocomotionClass> pLoco = loco.ToLocomotionClass<WalkLocomotionClass>();
								pLoco->IsReallyMoving = false;
							}
							else if (locoId == LocomotionClass.Mech)
							{
								Pointer<MechLocomotionClass> pLoco = loco.ToLocomotionClass<MechLocomotionClass>();
								pLoco->IsMoving = false;
							}
						}
						isMoving = false;
					}
				}
			}
*/
}

void Stand::RemoveStandIllegalTarget()
{
	AbstractClass* pTarget = pStand->Target;
	if (pTarget && !CanAttack(pStand, pTarget))
	{
		ClearAllTarget(pStand);
	}
}

void Stand::UpdateLocation(LocationMark locationMark)
{
	if (!locationMark.Location.IsEmpty() && !_isMoving)
	{
		_isMoving = _lastLocationMark.Location != locationMark.Location;
	}
	_lastLocationMark = locationMark;
	SetLocation(locationMark.Location);
	SetFacing(locationMark.Dir, false);
}

void Stand::SetLocation(CoordStruct location)
{
	pStand->SetLocation(location);
	if (!Data.IsTrain && Data.SameMoving && Data.StickOnFloor && !pStand->GetTechnoType()->JumpJet && pTechno->GetHeight() <= 0)
	{
		pStand->SetHeight(0);
	}
	pStand->SetFocus(nullptr);
}

void Stand::SetFacing(DirStruct dir, bool forceSetTurret)
{
	if (!Data.FreeDirection)
	{
		if (pStand->HasTurret() || Data.LockDirection)
		{
			// 替身有炮塔直接转身体
			pStand->PrimaryFacing.SetCurrent(dir);
		}

		// 检查是否需要同步转炮塔
		if ((!pStand->Target || Data.LockDirection) && !pStand->GetTechnoType()->TurretSpins)
		{
			// Logger.Log("设置替身{0}炮塔的朝向", Type.Type);
			if (forceSetTurret)
			{
				ForceFacing(dir);
			}
			else
			{
				if (pStand->HasTurret())
				{
					// TODO 炮塔限界
					// 炮塔的旋转交给炮塔旋转自己控制
					// TechnoStatus* status = nullptr;
					// if (!TryGetStatus<TechnoExt>(pStand, status) || !status->TurretAngleData.Enable)
					// {
					// 	pStand->SecondaryFacing.SetDesired(dir);
					// }
				}
				else
				{
					pStand->PrimaryFacing.SetDesired(dir);
				}
			}
		}
	}
}

void Stand::ForceFacing(DirStruct dir)
{
	pStand->PrimaryFacing.SetCurrent(dir);
	if (pStand->HasTurret())
	{
		// TODO 炮塔限界
		// TechnoStatus* status = nullptr;
		// if (TryGetStatus<TechnoExt>(pStand, status) && status->DefaultAngleIsChange(dir))
		// {
		// 	pStand->SecondaryFacing.SetCurrent(status->LockTurretDir);
		// }
		// else
		{
			pStand->SecondaryFacing.SetCurrent(dir);
		}
	}
}

void Stand::Start()
{
	CreateAndPutStand();
}

void Stand::End(CoordStruct location)
{
	if (pStand)
	{
		ExplodesOrDisappear(false);
	}
}

bool Stand::IsAlive()
{
	if (IsDead(pStand))
	{
		return false;
	}
	return true;
}

void Stand::OnGScreenRender(CoordStruct location)
{
	if (!standIsBuilding && IsFoot())
	{
		// synch tilt
		if (!Data.IsTrain)
		{
			if (Data.SameTilter)
			{
				// TODO Stand same tilter
			}
		}
	}
}

void Stand::OnPut(CoordStruct* pCoord, DirType dirType)
{
	if (pStand->InLimbo)
	{
		CoordStruct location = *pCoord;
		if (!TryPutTechno(pStand, location))
		{
			End(location);
		}
	}
}

void Stand::OnRemove()
{
	if (!AE->OwnerIsDead())
	{
		pStand->Limbo();
	}
}

void Stand::OnUpdate()
{
	if (pTechno)
	{
		UpdateStateTechno(AE->OwnerIsDead());
	}
	else
	{
		UpdateStateBullet();
	}
}

void Stand::OnWarpUpdate()
{
	if (pTechno)
	{
		UpdateStateTechno(AE->OwnerIsDead());
	}
	else
	{
		UpdateStateBullet();
	}
}

void Stand::OnTemporalEliminate(TemporalClass* pTemporal)
{
	End(pObject->GetCoords());
}

void Stand::OnReceiveDamageDestroy()
{
	onReceiveDamageDestroy = true;
	// 我不做人了JOJO
	notBeHuman = Data.ExplodesWithMaster;
	if (pTechno)
	{
		// 沉没，坠机，不销毁替身
		pStand->QueueMission(Mission::Sleep, true);
	}
	else if (pBullet)
	{
		// 抛射体上的宿主直接炸
		End(pBullet->GetCoords());
	}
}

void Stand::OnGuardCommand()
{
	if (!pStand->IsSelected)
	{
		// 执行替身的OnStop
		auto ext = TechnoExt::ExtMap.Find(pStand);
		ext->_GameObject->Foreach([](Component* c)
			{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnGuardCommand(); } });
	}
}

void Stand::OnStopCommand()
{
	ClearAllTarget(pStand);
	onStopCommand = true;
	if (!pStand->IsSelected)
	{
		pStand->ClickedEvent(NetworkEvents::Idle);
		// 执行替身的OnStop
		auto ext = TechnoExt::ExtMap.Find(pStand);
		ext->_GameObject->Foreach([](Component* c)
			{ if (auto cc = dynamic_cast<ITechnoScript*>(c)) { cc->OnStopCommand(); } });
	}
}

void Stand::OnRocketExplosion()
{
	if (!onReceiveDamageDestroy)
	{
		ExplodesOrDisappear(Data.ExplodesWithMaster);
	}

}

