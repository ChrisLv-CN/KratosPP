#include "TeleportState.h"

#include <AircraftTrackerClass.h>
#include <JumpjetLocomotionClass.h>
#include <TeleportLocomotionClass.h>

#include <Extension/TechnoTypeExt.h>
#include <Extension/WarheadTypeExt.h>

#include <Ext/Helper/Scripts.h>

#include <Ext/TechnoType/TechnoStatus.h>
#include <Ext/ObjectType/AttachEffect.h>

bool TeleportState::Teleport(CoordStruct* pLocation, WarheadTypeClass* pWH)
{
	if (IsAlive() && _step == TeleportStep::READY)
	{
		CoordStruct targetPos = *pLocation;
		if (!targetPos.IsEmpty())
		{
			WarheadTypeExt::TypeData* typeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
			if (typeData && typeData->Teleporter)
			{
				_warpTo = targetPos;
				return true;
			}
		}
	}
	return false;
}

bool TeleportState::IsFreezing()
{
	bool freeze = false;
	switch (_step)
	{
	case TeleportStep::TELEPORTED:
	case TeleportStep::FREEZING:
		freeze = true;
		break;
	}
	return freeze;
}

bool TeleportState::IsReadyToMoveWarp()
{
	return IsReady() && _step == TeleportStep::READY && Data.Mode != TeleportMode::WARHEAD;
}

void TeleportState::Reload()
{
	if (_delay > 0)
	{
		_delayTimer.Start(_delay);
	}
	_count++;
}

bool TeleportState::IsReady()
{
	return !IsDone() && Timeup() && _canWarp;
}

bool TeleportState::Timeup()
{
	return _delay <= 0 || _delayTimer.Expired();
}

bool TeleportState::IsDone()
{
	return Data.TriggeredTimes > 0 && _count >= Data.TriggeredTimes;
}

void TeleportState::OnStart()
{
	_count = 0;
	_delay = Data.Delay;
	TechnoStatus* status = dynamic_cast<TechnoStatus*>(_parent);
	if (status && status->AmIStand())
	{
		End();
		return;
	}
	_canWarp = Data.Enable;
}

void TeleportState::OnEnd()
{
	_canWarp = false;
}

CoordStruct TeleportState::GetAndMarkDestination(CoordStruct location)
{
	CoordStruct targetPos = CoordStruct::Empty;
	pDest = nullptr;
	pFocus = nullptr;

	FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
	// 没有被磁电抬起
	if (!pFoot->IsAttackedByLocomotor)
	{
		// 移动到自身相对位置
		if (!Data.MoveTo.IsEmpty())
		{
			targetPos = GetFLHAbsoluteCoords(pTechno, Data.MoveTo, Data.IsOnTurret);
		}
		else
		{
			// 是否正在移动, Aircraft pFoot->GetCurrentSpeed always is Zero
			pFoot->Locomotor->Destination(&targetPos);
			// 子机导弹不一定具有移动目的地，有目标时，亦可使用目标位置作为跳跃点位置
			if (targetPos.IsEmpty()
				&& pTechno->WhatAmI() == AbstractType::Aircraft
				&& pTechno->GetTechnoType()->MissileSpawn
				&& pTechno->Target)
			{
				targetPos = pTechno->Target->GetCoords();
			}
		}
		// 目的地和本体位置在同一格内就不跳
		CellStruct s = CellClass::Coord2Cell(location);
		CellStruct t = CellClass::Coord2Cell(targetPos);
		if (s.X == t.X && s.Y == t.Y)
		{
			// Same cell, don't move
			targetPos = CoordStruct::Empty;
		}
		if (!targetPos.IsEmpty())
		{
			// 记录下目的地
			pDest = pFoot->Destination;
			pFocus = pTechno->Focus;
		}
	}
	return targetPos;
}

void TeleportState::OnUpdate()
{
	if (!IsBuilding() && !IsDeadOrInvisible(pTechno))
	{
		CoordStruct location = pTechno->GetCoords();
		switch (_step)
		{
		case TeleportStep::READY:
		{
			if (IsDone())
			{
				End();
				return;
			}
			if (IsReady())
			{
				CoordStruct targetPos = GetAndMarkDestination(location);
				switch (Data.Mode)
				{
				case TeleportMode::MOVE:
					break;
				case TeleportMode::WARHEAD:
					targetPos = _warpTo;
					_warpTo = CoordStruct::Empty;
					break;
				case TeleportMode::BOTH:
					if (!_warpTo.IsEmpty())
					{
						targetPos = _warpTo;
						_warpTo = CoordStruct::Empty;
					}
					break;
				default:
					return;
				}
				if (!targetPos.IsEmpty())
				{
					// 传送距离检查
					double distance = targetPos.DistanceFrom(location);
					if (distance > Data.RangeMin * Unsorted::LeptonsPerCell && (Data.RangeMax < 0 ? true : distance < Data.RangeMax * Unsorted::LeptonsPerCell))
					{
						// 在可以传送的范围内
						double dist = Data.Distance * Unsorted::LeptonsPerCell;
						if (dist > 0 && distance > dist)
						{
							// 有限距离的传送，重新计算目标位置
							targetPos = GetForwardCoords(location, targetPos, dist, distance);
						}
					}
					else
					{
						// 不可以传送
						targetPos = CoordStruct::Empty;
					}
				}
				if (!targetPos.IsEmpty())
				{
					// 跳跃位置偏移
					if (!Data.Offset.IsEmpty())
					{
						DirStruct facing{};
						AbstractClass* pTarget = pTechno->Target;
						TechnoClass* pTargetTechno = nullptr;
						if (Data.IsOnTarget && CastToTechno(pTarget, pTargetTechno))
						{
							if (pTargetTechno->WhatAmI() == AbstractType::Aircraft || (Data.IsOnTurret && pTargetTechno->HasTurret()))
							{
								facing = pTargetTechno->SecondaryFacing.Current();
							}
							else if (pTargetTechno->GetTechnoType()->Locomotor == LocomotionClass::CLSIDs::Jumpjet)
							{
								FootClass* pTargetFoot = dynamic_cast<FootClass*>(pTargetTechno);
								if (JumpjetLocomotionClass* jjLoco = dynamic_cast<JumpjetLocomotionClass*>(pTargetFoot->Locomotor.get()))
								{
									facing = jjLoco->LocomotionFacing.Current();
								}
							}
							else
							{
								facing = pTargetTechno->PrimaryFacing.Current();
							}
						}
						else
						{
							facing = Point2Dir(location, targetPos);
						}
						targetPos = GetFLHAbsoluteCoords(targetPos, Data.Offset, facing);
					}
					// 检查目的地是否可以着陆
					bool isJJ = pTechno->GetTechnoType()->Locomotor == LocomotionClass::CLSIDs::Jumpjet;
					CellClass* pTargetCell = nullptr;
					if (CellClass* pCell = MapClass::Instance->TryGetCellAt(targetPos))
					{
						if (pTechno->IsInAir())
						{
							pTargetCell = pCell;
						}
						else
						{
							TechnoTypeClass* pType = pTechno->GetTechnoType();

							int times = 0;
							do
							{
								bool canEnterCell = false;
								Move move = pTechno->IsCellOccupied(pCell, -1, -1, nullptr, false);
								switch (move)
								{
								case Move::OK:
									// case Move::MovingBlock:
									canEnterCell = true;
									break;
								}
								if (isJJ)
								{
									canEnterCell = pCell->Jumpjet == nullptr;
								}
								bool canMoveTo = pCell->IsClearToMove(pType->SpeedType, pType->MovementZone, true, true) && canEnterCell;
								if (canMoveTo)
								{
									pTargetCell = pCell;
									break;
								}
								CellStruct curretCell = pCell->MapCoords;
								int zone = MapClass::Instance->GetMovementZoneType(curretCell, pType->MovementZone, pTechno->IsOnBridge());
								bool alt = (bool)(pCell->Flags & CellFlags::CenterRevealed);
								CellStruct nextCell = MapClass::Instance->NearByLocation(curretCell, pType->SpeedType, zone, pType->MovementZone, alt, 1, 1, 0, true, false, true, curretCell, false, false);
								pCell = MapClass::Instance->TryGetCellAt(nextCell);
							} while (pCell && times++ < 9);
						}
					}
					// 可以跳
					if (pTargetCell)
					{
						FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
						targetPos = pTargetCell->GetCoordsWithBridge();
						_teleportTimer.Stop();
						if (Data.ClearTarget)
						{
							ClearAllTarget(pTechno);
						}
						// Warp
						if (pTechno->IsInAir() || pTechno->WhatAmI() == AbstractType::Aircraft)
						{
							// 空中跳，自定义跳
							int height = pTechno->GetHeight() + Data.Offset.Z;
							if (CellClass* pSourceCell = MapClass::Instance->TryGetCellAt(location))
							{
								if (pSourceCell->ContainsBridge())
								{
									height -= pSourceCell->BridgeHeight;
								}
							}
							targetPos.Z += height;
							if (isJJ)
							{
								ForceStopMoving(pFoot);
								pFoot->Locomotor->Force_Track(-1, location);
								pFoot->FrozenStill = true;
								pFoot->SendToEachLink(RadioCommand::NotifyUnlink);
							}
							// 移动位置
							pTechno->UpdatePlacement(PlacementType::Remove);
							pTechno->SetLocation(targetPos);
							pTechno->UpdatePlacement(PlacementType::Put);
							if (isJJ)
							{
								pFoot->Jumpjet_OccupyCell(pTargetCell->MapCoords);
							}
							// 设置面向
							pTechno->PrimaryFacing.SetCurrent(pTechno->PrimaryFacing.Current());
							pTechno->SecondaryFacing.SetCurrent(pTechno->SecondaryFacing.Current());
							// 移除黑幕
							MapClass::Instance->RevealArea2(&targetPos, pTechno->LastSightRange, pTechno->Owner, false, false, false, true, 0);
							MapClass::Instance->RevealArea2(&targetPos, pTechno->LastSightRange, pTechno->Owner, false, false, false, true, 1);

							// 播放自定义传送动画
							TechnoTypeExt::TypeData* typeData = GetTypeData<TechnoTypeExt, TechnoTypeExt::TypeData>(pTechno->GetTechnoType());
							AnimTypeClass* pWarpOut = nullptr;
							AnimTypeClass* pWarpIn = nullptr;
							if (IsNotNone(typeData->WarpOut))
							{
								pWarpOut = AnimTypeClass::Find(typeData->WarpOut.c_str());
							}
							else
							{
								pWarpOut = RulesClass::Instance->WarpOut;
							}
							if (pWarpOut)
							{
								AnimClass* pAnimOut = GameCreate<AnimClass>(pWarpOut, location);
								SetAnimOwner(pAnimOut, pTechno);
							}
							if (IsNotNone(typeData->WarpIn))
							{
								pWarpIn = AnimTypeClass::Find(typeData->WarpIn.c_str());
							}
							else
							{
								pWarpIn = RulesClass::Instance->WarpIn;
							}
							if (pWarpIn)
							{
								AnimClass* pAnimIn = GameCreate<AnimClass>(pWarpIn, targetPos);
								SetAnimOwner(pAnimIn, pTechno);
							}
							// 播放声音
							int outSound = pTechno->GetTechnoType()->ChronoOutSound;
							if (outSound >= 0 || (outSound = RulesClass::Instance->ChronoOutSound) >= 0)
							{
								VocClass::PlayAt(outSound, location);
							}
							int inSound = pTechno->GetTechnoType()->ChronoInSound;
							if (inSound >= 0 || (inSound = RulesClass::Instance->ChronoInSound) >= 0)
							{
								VocClass::PlayAt(inSound, targetPos);
							}
							// 传送冷冻
							if (Data.FreezingInAir)
							{
								int delay = typeData->ChronoMinimumDelay;
								if (typeData->ChronoTrigger)
								{
									// 根据传送距离计算时间
									double distance = targetPos.DistanceFrom(location);
									if (distance > typeData->ChronoRangeMinimum)
									{
										int factor = std::max(typeData->ChronoDistanceFactor, 1);
										delay = (int)(distance / factor);
									}
								}
								pTechno->WarpingOut = true;
								_teleportTimer.Start(delay);
							}
						}
						else if (Data.Super)
						{
							// 使用超武跳
							pFoot->ChronoWarpTo(targetPos);
						}
						else
						{
							// 普通跳
							ForceStopMoving(pFoot);
							// 清除当前格子的占据
							pFoot->Locomotor->Force_Track(-1, location);
							pFoot->FrozenStill = true;
							if (!dynamic_cast<TeleportLocomotionClass*>(pFoot->Locomotor.get()))
							{
								LocomotionClass::ChangeLocomotorTo(pFoot, LocomotionClass::CLSIDs::Teleport);
							}
							// 移动到目的地
							// pFoot->IsImmobilized = true;
							// pFoot->ChronoDestCoords = targetPos;
							// pFoot->SendToEachLink(RadioCommand::NotifyUnlink);
							// pFoot->ChronoWarpedByHouse = pTechno->Owner;
							pFoot->SetDestination(pTargetCell, true);

							// 传送冷冻
							TechnoTypeExt::TypeData* typeData = GetTypeData<TechnoTypeExt, TechnoTypeExt::TypeData>(pTechno->GetTechnoType());
							int delay = typeData->ChronoMinimumDelay;
							if (typeData->ChronoTrigger)
							{
								// 根据传送距离计算时间
								double distance = targetPos.DistanceFrom(location);
								if (distance > typeData->ChronoRangeMinimum)
								{
									int factor = std::max(typeData->ChronoDistanceFactor, 1);
									delay = (int)(distance / factor);
								}
							}
							pTechno->WarpingOut = true;
							_teleportTimer.Start(delay);
						}
						// 通知AE管理器进行了跳跃
						if (AttachEffect* aeManager = GetAEManager<TechnoExt>(pTechno))
						{
							aeManager->ClearLocationMarks();
						}
						// 进入下一阶段
						Reload();
						_step = TeleportStep::TELEPORTED;
					}
				}
			}
			break;
		}
		case TeleportStep::TELEPORTED:
		{
			if (Data.Super && !pTechno->IsInAir())
			{
				// 超武跳，不用冷冻计时器
				if (!pTechno->WarpingOut)
				{
					_step = TeleportStep::MOVEFORWARD;
				}
			}
			else
			{
				if (_teleportTimer.Expired())
				{
					// 解冻，进入下一个阶段
					pTechno->WarpingOut = false;
					_step = TeleportStep::MOVEFORWARD;
				}
				else
				{
					_step = TeleportStep::FREEZING;
				}
			}
			break;
		}
		case TeleportStep::FREEZING:
		{
			if (_teleportTimer.Expired() || !pTechno->WarpingOut)
			{
				// 解冻，进入下一个阶段
				pTechno->WarpingOut = false;
				_step = TeleportStep::MOVEFORWARD;
			}
			break;
		}
		case TeleportStep::MOVEFORWARD:
		{
			_step = TeleportStep::READY;
			CellClass* pCell = MapClass::Instance->TryGetCellAt(location);
			// 空中单位需要更新在空中的追踪位置，关系到防空武器的命中判定
			FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
			if (pTechno->IsInAir() && pCell)
			{
				AircraftTrackerClass::Instance->Update_Entry(pTechno, pFoot->LastJumpjetMapCoords, pCell->MapCoords);
			}
			if (!pTechno->Target)
			{
				if (Data.MoveForward)
				{
					// 把移动目的地，设回去
					pTechno->SetFocus(pFocus);
					pTechno->SetDestination(pDest, true);
					pTechno->QueueMission(Mission::Move, true);
				}
				else if (pCell)
				{
					pTechno->SetDestination(pCell, true);
					pTechno->QueueMission(Mission::Move, true);
				}
			}
			else
			{
				pTechno->QueueMission(Mission::Attack, true);
			}
			pFocus = nullptr;
			pDest = nullptr;
			break;
		}
		}
	}
}
