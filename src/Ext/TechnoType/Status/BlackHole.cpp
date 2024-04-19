#include "../TechnoStatus.h"

#include <JumpjetLocomotionClass.h>

#include <Ext/Helper/Physics.h>
#include <Ext/Helper/Weapon.h>
#include <Ext/Helper/Scripts.h>

#include <Extension/WarheadTypeExt.h>

void TechnoStatus::BlackHoleCapture(ObjectClass* pBlackHole, BlackHoleData data)
{
	if (!CaptureByBlackHole || !_blackHoleData.Enable || _blackHoleData.Weight < data.Weight)
	{
		CaptureByBlackHole = true;
		_pBlackHole = pBlackHole;
		_blackHoleData = data;
	}
}

void TechnoStatus::BlackHoleCancel()
{
	if (CaptureByBlackHole && !IsBuilding() && !IsDeadOrInvisible(pTechno))
	{
		FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
		pFoot->Locomotor->Unlock();
		// 恢复可控制
		if (_lostControl)
		{
			pFoot->ForceMission(Mission::Guard);
		}
		// 摔死
		int fallingDestroyHeight = 0;
		if (_blackHoleData.AllowFallingDestroy)
		{
			fallingDestroyHeight = _blackHoleData.FallingDestroyHeight;
		}
		FallingExceptAircraft(pTechno, fallingDestroyHeight, false);
	}
	CaptureByBlackHole = false;
	_pBlackHole = nullptr;
	_blackHoleData.Enable = false;
	_lostControl = false;
}

void TechnoStatus::OnUpdate_BlackHole()
{
	if (CaptureByBlackHole)
	{
		CoordStruct location = pTechno->GetCoords();
		BlackHoleState* blackHoleState = nullptr;
		AttachEffect* aem = AEManager();
		if (IsDeadOrInvisible(_pBlackHole)
			|| !TryGetBlackHoleState(_pBlackHole, blackHoleState)
			|| !blackHoleState->IsAlive()
			|| blackHoleState->IsOutOfRange(location)
			|| !aem || !aem->IsOnMark(_blackHoleData))
		{
			BlackHoleCancel();
		}
		else
		{
			// 黑洞伤害，排除我自己不是黑洞
			if (_blackHoleData.AllowDamageTechno && _blackHoleData.Damage != 0 && !BlackHole->IsAlive())
			{
				if (_blackHoleDamageDelay.Expired())
				{
					_blackHoleDamageDelay.Start(_blackHoleData.DamageDelay);
					WarheadTypeClass* pWH = RulesClass::Instance->C4Warhead;
					if (IsNotNone(_blackHoleData.DamageWH))
					{
						pWH = WarheadTypeClass::Find(_blackHoleData.DamageWH.c_str());
						if (!pWH)
						{
							Debug::Log("Warning: A Blackhole want take damage to Techno but not have warhead [%s]\n", _blackHoleData.DamageWH.c_str());
						}
					}
					if (pWH)
					{
						ObjectClass* pAttacker = nullptr;
						HouseClass* pAttackingHouse = nullptr;
						BulletClass* pBlackHoleBullet = nullptr;
						if (CastToBullet(_pBlackHole, pBlackHoleBullet))
						{
							pAttacker = pBlackHoleBullet->Owner;
							pAttackingHouse = GetSourceHouse(pBlackHoleBullet);
						}
						else
						{
							pAttacker = _pBlackHole;
							pAttackingHouse = _pBlackHole->GetOwningHouse();
						}
						pTechno->TakeDamage(_blackHoleData.Damage, pWH, pTechno->GetTechnoType()->Crewed, false, pAttacker, pAttackingHouse);
					}
				}
			}
			// 目标设置
			if (_blackHoleData.ClearTarget)
			{
				ClearAllTarget(pTechno);
			}
			if (_blackHoleData.ChangeTarget)
			{
				pTechno->SetTarget(_pBlackHole);
			}
			if (!IsBuilding())
			{
				// 失控设置
				if (_blackHoleData.OutOfControl)
				{
					_lostControl = true;
					ClearAllTarget(pTechno);
					pTechno->Deselect();
					pTechno->ForceMission(Mission::None);
					pTechno->QueueMission(Mission::Sleep, false);
				}
				// 移动位置
				// 从占据的格子中移除自己
				pTechno->UnmarkAllOccupationBits(location);
				FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
				// 停止移动
				ForceStopMoving(pFoot);
				// 计算下一个坐标点
				// 以偏移量为FLH获取目标点
				CoordStruct targetPos = GetFLHAbsoluteCoords(_pBlackHole, _blackHoleData.Offset, _blackHoleData.IsOnTurret);
				CoordStruct nextPos = targetPos;
				double dist = targetPos.DistanceFrom(location);
				// 获取捕获速度
				int speed = _blackHoleData.GetCaptureSpeed(pTechno->GetTechnoType()->Weight);
				if (dist > speed)
				{
					// 计算下一个坐标
					nextPos = GetForwardCoords(location, targetPos, speed);
				}
				CoordStruct nextCellPos = CoordStruct::Empty;
				bool onBridge = pTechno->OnBridge;
				PassError passError = CanMoveTo(location, nextPos, _blackHoleData.AllowPassBuilding, nextCellPos, onBridge);
				switch (passError)
				{
				case PassError::HITWALL:
				case PassError::HITBUILDING:
				case PassError::UPBRIDEG:
					// 反弹回移动前的格子
					if (CellClass* pSourceCell = MapClass::Instance->TryGetCellAt(location))
					{
						CoordStruct cellPos = pSourceCell->GetCoordsWithBridge();
						nextPos.X = cellPos.X;
						nextPos.Y = cellPos.Y;
						if (nextPos.Z < cellPos.Z)
						{
							nextPos.Z = cellPos.Z;
						}
					}
					break;
				case PassError::UNDERGROUND:
				case PassError::DOWNBRIDGE:
					// 卡在地表
					nextPos.Z = nextCellPos.Z;
					break;
				}
				// 被黑洞吸走
				pTechno->UpdatePlacement(PlacementType::Remove);
				// 是否在桥上
				pTechno->OnBridge = onBridge;
				pTechno->SetLocation(nextPos);
				pTechno->UpdatePlacement(PlacementType::Put);
				// 移除黑幕
				MapClass::Instance->RevealArea2(&nextPos, pTechno->LastSightRange, pTechno->Owner, false, false, false, true, 0);
				MapClass::Instance->RevealArea2(&nextPos, pTechno->LastSightRange, pTechno->Owner, false, false, false, true, 1);
				// 设置动作
				if (_blackHoleData.AllowCrawl && IsInfantry())
				{
					dynamic_cast<InfantryClass*>(pTechno)->PlayAnim(Sequence::Crawl);
				}
				// 设置翻滚
				if (_blackHoleData.AllowRotateUnit)
				{
					// 设置朝向
					if (_lastMission == Mission::Move || _lastMission == Mission::AttackMove || pTechno->GetTechnoType()->ConsideredAircraft || !pTechno->IsInAir())
					{
						CoordStruct p1 = targetPos;
						CoordStruct p2 = location;
						p1.Z = 0;
						p2.Z = 0;
						if (p1.DistanceFrom(p2) >= speed)
						{
							DirStruct facingDir = Point2Dir(targetPos, location);
							pTechno->PrimaryFacing.SetDesired(facingDir);
							if (IsJumpjet())
							{
								// JJ朝向是单独的Facing
								if (JumpjetLocomotionClass* jjLoco = dynamic_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get()))
								{
									jjLoco->LocomotionFacing.SetDesired(facingDir);
								}
							}
							else if (IsAircraft())
							{
								// 飞机使用的炮塔的Facing
								pTechno->SecondaryFacing.SetDesired(facingDir);
							}
						}
					}
				}
			}
		}
	}
}

void TechnoStatus::OnReceiveDamageEnd_BlackHole(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse)
{
	if (!IsDeadOrInvisible(pTechno) && damageState != DamageState::NowDead && !IsDeadOrInvisible(pAttacker))
	{
		WarheadTypeExt::TypeData* whData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
		if (whData && whData->Capturer)
		{
			AttachEffect* aem = AEManager();
			BlackHoleState* blackHoleState = GetBlackHoleState(pAttacker);
			if (blackHoleState && blackHoleState->IsAlive())
			{
				BlackHoleData data = blackHoleState->Data;
				if (data.Enable && data.CaptureFromWarhead
					&& !blackHoleState->IsOutOfRange(pTechno->GetCoords())
					&& data.CanAffectType(pTechno)
					&& (!aem || aem->IsOnMark(data)))
				{
					BlackHoleCapture(pAttacker, data);
				}
			}
		}
	}
}
