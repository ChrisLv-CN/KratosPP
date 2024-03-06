#include "TeleportState.h"

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
	return !IsDone() && Timeup();
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
	TechnoStatus* status = GetStatus<TechnoExt, TechnoStatus>(pTechno);
	if (status && status->AmIStand())
	{
		End();
	}
}

CoordStruct TeleportState::GetAndMarkDestination()
{
	CoordStruct targetPos = CoordStruct::Empty;
	FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
	// 是否正在移动
	if (pFoot->Locomotor->Is_Moving() && pFoot->GetCurrentSpeed() > 0)
	{
		pFoot->Locomotor->Destination(&targetPos);
		// 记录下目的地
		pDest = pFoot->Destination;
		pFocus = pTechno->Focus;
	}
	// 移动到自身相对位置
	if (!Data.MoveTo.IsEmpty())
	{
		targetPos = GetFLHAbsoluteCoords(pTechno, Data.MoveTo, Data.IsOnTurret);
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
				CoordStruct targetPos = CoordStruct::Empty;
				switch (Data.Mode)
				{
				case TeleportMode::MOVE:
					targetPos = GetAndMarkDestination();
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
					else
					{
						targetPos = GetAndMarkDestination();
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
				if (bool teleporting = !targetPos.IsEmpty())
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
								FootClass* pFoot = dynamic_cast<FootClass*>(pTargetTechno);
								JumpjetLocomotionClass* jjLoco = static_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get());
								facing = jjLoco->LocomotionFacing.Current();
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
					if (!Data.Super)
					{
						if (CellClass* pCell = MapClass::Instance->TryGetCellAt(targetPos))
						{
							TechnoTypeClass* pType = pTechno->GetTechnoType();
							if (!pCell->IsClearToMove(pType->SpeedType, pType->MovementZone, true, true))
							{
								// 不能通过，需要找一个新的落脚点
								teleporting = false;
							}
						}
					}
					// 可以跳
					if (teleporting)
					{
						_loco = nullptr;
						_teleportTimer.Stop();
						if (Data.ClearTarget)
						{
							ClearAllTarget(pTechno);
						}
						// Warp
						if (pTechno->IsInAir())
						{
							// 空中跳，自定义跳
							int height = pTechno->GetHeight();
							targetPos.Z += height;
							// 移动位置
							pTechno->UpdatePlacement(PlacementType::Remove);
							pTechno->SetLocation(targetPos);
							pTechno->UpdatePlacement(PlacementType::Put);
							// 移除黑幕
							MapClass::Instance->RevealArea2(&targetPos, pTechno->LastSightRange, pTechno->Owner, false, false, false, true, 0);
							MapClass::Instance->RevealArea2(&targetPos, pTechno->LastSightRange, pTechno->Owner, false, false, false, true, 1);

							// 播放自定义传送动画
							TechnoTypeExt::TypeData* typeData = GetTypeData<TechnoTypeExt, TechnoTypeExt::TypeData>(pTechno->GetTechnoType());
							AnimTypeClass* pAnimType = nullptr;
							if (IsNotNone(typeData->WarpOut))
							{
								pAnimType = AnimTypeClass::Find(typeData->WarpOut.c_str());
							}
							else
							{
								pAnimType = RulesClass::Instance->WarpOut;
							}
							if (pAnimType)
							{
								AnimClass* pAnimOut = GameCreate<AnimClass>(pAnimType, location);
								SetAnimOwner(pAnimOut, pTechno);

								AnimClass* pAnimIn = GameCreate<AnimClass>(pAnimType, targetPos);
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
							// 空中无法进行冷冻，JJ和飞机依旧会乱跑
							if (pTechno->GetTechnoType()->Locomotor != LocomotionClass::CLSIDs::Jumpjet && !IsAircraft())
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
							dynamic_cast<FootClass*>(pTechno)->ChronoWarpTo(targetPos);
						}
						else
						{
							// 普通跳
							FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
							ForceStopMoving(pFoot);
							LocomotionClass::ChangeLocomotorTo(pFoot, LocomotionClass::CLSIDs::Teleport);
							_loco = pFoot->Locomotor.get();
							_loco->Move_To(targetPos);
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
					if (Data.MoveForward)
					{
						_step = TeleportStep::MOVEFORWARD;
					}
					else
					{
						_step = TeleportStep::READY;
					}
				}
			}
			else
			{
				if (_loco)
				{
					// 当前帧切换loco后会切回来，而且下一帧才可以获得计时器
					_teleportTimer = static_cast<TeleportLocomotionClass*>(_loco)->Timer;
				}
				if (_teleportTimer.Expired())
				{
					// 解冻，进入下一个阶段
					pTechno->WarpingOut = false;
					if (Data.MoveForward)
					{
						_step = TeleportStep::MOVEFORWARD;
					}
					else
					{
						_step = TeleportStep::READY;
					}
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
				if (Data.MoveForward)
				{
					_step = TeleportStep::MOVEFORWARD;
				}
				else
				{
					_step = TeleportStep::READY;
				}
			}
			break;
		}
		case TeleportStep::MOVEFORWARD:
		{
			_step = TeleportStep::READY;
			if (!pTechno->Target)
			{
				// 把移动目的地，设回去
				if (pFocus)
				{
					pTechno->SetFocus(pFocus);
				}
				if (pDest)
				{
					pTechno->SetDestination(pDest, true);
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
