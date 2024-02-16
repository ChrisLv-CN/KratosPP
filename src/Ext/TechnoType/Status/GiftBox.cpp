#include "../TechnoStatus.h"

#include <FootClass.h>
#include <JumpjetLocomotionClass.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/ObjectType/AttachEffect.h>


DeployToTransformData* TechnoStatus::GetTransformData()
{
	if (!_transformData)
	{
		_transformData = INI::GetConfig<DeployToTransformData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _transformData;
}

void TechnoStatus::InitState_GiftBox()
{
	// 读取单位身上的礼盒设置
	GiftBoxData* data = INI::GetConfig<GiftBoxData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	if (data->Enable)
	{
		GiftBoxState.Enable(*data);
	}
}

void TechnoStatus::OnUpdate_DeployToTransform()
{
	if (GetTransformData()->Enable)
	{
		if ((IsInfantry() && static_cast<InfantryClass*>(pTechno)->SequenceAnim == Sequence::Deployed)
			|| (IsUnit() && static_cast<UnitClass*>(pTechno)->Deployed))
		{
			// 步兵或载具部署完毕，开始变形
			GiftBoxState.Enable(*GetTransformData());
		}
	}
}

void TechnoStatus::OnUpdate_GiftBox()
{
	GiftBoxState.Update(pTechno->Veterancy.IsElite());
	// 记录单位的状态
	if (GiftBoxState.IsActive())
	{
		// 记录盒子的状态
		GiftBoxState.IsSelected = pTechno->IsSelected;
		GiftBoxState.Group = pTechno->Group;
		// 记录朝向
		GiftBoxState.BodyDir = pTechno->PrimaryFacing.Current();
		GiftBoxState.TurretDir = pTechno->SecondaryFacing.Current();
		// JJ有单独的Facing
		if (IsJumpjet())
		{
			FootClass* pFoot = static_cast<FootClass*>(pTechno);
			GiftBoxState.BodyDir = static_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get())->LocomotionFacing.Current();
			GiftBoxState.TurretDir = GiftBoxState.BodyDir;
		}

		// 准备开盒
		if (GiftBoxState.CanOpen() && IsOnMark_GiftBox() && !GiftBoxState.Data.OpenWhenDestroyed && !GiftBoxState.Data.OpenWhenHealthPercent)
		{
			// 开盒
			GiftBoxState.IsOpen = true;
			// 释放礼物
			std::vector<std::string> gifts = GiftBoxState.GetGiftList();
			if (!gifts.empty())
			{
				ReleaseGift(gifts, GiftBoxState.Data);
			}
		}

		// 重置或者销毁盒子
		if (GiftBoxState.IsOpen)
		{
			if (GiftBoxState.Data.Remove)
			{
				GiftBoxState.Disable();
				if (GiftBoxState.Data.Destroy)
				{
					pTechno->TakeDamage(pTechno->Health + 1, pTechno->GetTechnoType()->Crewed);
				}
				else
				{
					pTechno->Health = 0;
					pTechno->Limbo();
					pTechno->UnInit();
				}
			}
			else
			{
				GiftBoxState.ResetGiftBox();
			}
		}
	}

}

void TechnoStatus::OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	if (damageState != DamageState::NowDead && IsDeadOrInvisible(pTechno)
		&& GiftBoxState.CanOpen() && GiftBoxState.Data.OpenWhenHealthPercent
		&& IsOnMark_GiftBox()
		)
	{
		// 计算血量百分比是否达到开启条件
		double healthPercent = pTechno->GetHealthPercentage();
		if (healthPercent <= GiftBoxState.Data.OpenHealthPercent)
		{
			// 开盒
			GiftBoxState.IsOpen = true;
			// 释放礼物
			std::vector<std::string> gifts = GiftBoxState.GetGiftList();
			if (!gifts.empty())
			{
				ReleaseGift(gifts, GiftBoxState.Data);
			}
			// 此处不重置或销毁，而是进入下一帧的Update事件中重置或销毁
		}
	}
}

void TechnoStatus::OnReceiveDamageDestroy_GiftBox()
{
	if (GiftBoxState.IsActive() && GiftBoxState.Data.OpenWhenDestroyed && IsOnMark_GiftBox())
	{
		// 开盒
		GiftBoxState.IsOpen = true;
		// 释放礼物
		std::vector<std::string> gifts = GiftBoxState.GetGiftList();
		if (!gifts.empty())
		{
			ReleaseGift(gifts, GiftBoxState.Data);
		}
	}
}

bool TechnoStatus::IsOnMark_GiftBox()
{
	// TODO AE isOnMark
	return true;
}

void TechnoStatus::ReleaseGift(std::vector<std::string> gifts, GiftBoxData data)
{
	HouseClass* pHouse = nullptr;
	// TODO AE ReceiverOwn
	if (false)
	{

	}
	else
	{
		pHouse = pTechno->Owner;
	}
	CoordStruct location;
	if (data.RealCoords)
	{
		location = pTechno->GetRenderCoords();
	}
	else
	{
		location = pTechno->GetCoords();
	}
	Mission currentMission = pTechno->CurrentMission;

	// 获取投送单位的位置
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		AbstractClass* pDest = nullptr; // 载具当前的移动目的地
		AbstractClass* pFocus = nullptr; // 步兵当前的移动目的地
		// 获取目的地
		if (!IsBuilding())
		{
			pDest = static_cast<FootClass*>(pTechno)->Destination;
			pFocus = pTechno->Focus;
		}
		// 读取盒子的状态
		int healthNumber = pTechno->Health;
		double healthPercent = pTechno->GetHealthPercentage();
		healthPercent = healthPercent <= 0 ? 1 : healthPercent; // 盒子死了，继承的血量就是满的
		bool changeHealth = data.IsTransform || data.InheritHealth; // Transform强制继承
		if (!changeHealth && data.HealthPercent > 0)
		{
			// 强设血量比例
			healthPercent = data.HealthPercent;
			changeHealth = true;
		}
		AbstractClass* pTarget = pTechno->Target;
		bool scatter = !data.Remove || data.ForceMission == Mission::Move;
		bool inheritAE = data.Remove && data.InheritAE;
		// 随机投送位置
		CellStruct cellPos = pCell->MapCoords;
		std::vector<CellStruct> cellOffsets{};
		if (data.RandomRange > 0)
		{
			auto const range = static_cast<size_t>(data.RandomRange + 0.99);
			for (CellSpreadEnumerator it(data.RandomRange); it; ++it) {
				CellStruct pos = *it;
				cellOffsets.push_back(pos);
			}
		}
		// 开始投送单位，每生成一个单位就选择一次位置
		for (std::string id : gifts)
		{
			// 投送单位
			TechnoTypeClass* pGiftType = nullptr; // 礼物的类型
			TechnoClass* pGift = nullptr; // 礼物本体
			CoordStruct putLocation = location; // 礼物的投放位置
			CellClass* pPutCell = pCell; // 礼物投放的格子
			if (RealReleaseGift(id, pHouse,
				location, pCell, cellOffsets, data.EmptyCell,
				pGiftType, pGift,
				putLocation, pPutCell)
				)
			{
				TechnoStatus* giftStatus = GetStatus<TechnoExt, TechnoStatus>(pGift);
				if (data.IsTransform)
				{
					// 同步朝向
					pGift->PrimaryFacing.SetCurrent(GiftBoxState.BodyDir);
					pGift->SecondaryFacing.SetCurrent(GiftBoxState.TurretDir);
					// JJ有单独的Facing
					if (giftStatus->IsJumpjet())
					{
						FootClass* pGiftFoot = static_cast<FootClass*>(pGift);
						JumpjetLocomotionClass* pLoco = static_cast<JumpjetLocomotionClass*>(pGiftFoot->Locomotor.get());
						pLoco->LocomotionFacing.SetCurrent(GiftBoxState.BodyDir);
					}
					// 同步编队
					pGift->Group = GiftBoxState.Group;
					// 同步箱子属性
					giftStatus->CrateBuff = CrateBuff;
				}

				// 同步选中状态
				if (GiftBoxState.IsSelected)
				{
					giftStatus->DisableSelectVoice = true;
					pGift->Select();
					giftStatus->DisableSelectVoice = false;
				}

				// 修改血量
				if (changeHealth)
				{
					int strength = pGiftType->Strength;
					int health = 0;
					if (data.InheritHealthNumber)
					{
						// 直接继承血量数字
						health = healthNumber;
					}
					else if (data.HealthNumber > 0)
					{
						// 直接赋予指定血量
						health = data.HealthNumber;
					}
					else
					{
						// 按比例计算血量
						health = (int)(strength * healthPercent);
					}
					if (health <= 0)
					{
						health = 1;
					}
					if (health < strength)
					{
						pGift->Health = health;
					}
				}

				// 继承等级
				if (data.InheritExperience && pGiftType->Trainable)
				{
					pGift->Veterancy = pTechno->Veterancy;
				}

				// 继承ROF
				if (data.InheritROF && pTechno->ROFTimer.InProgress())
				{
					pGift->ROFTimer.Start(pTechno->ROFTimer.GetTimeLeft());
				}

				// 继承弹药
				if (data.InheritAmmo && pGiftType->Ammo > 1 && pTechno->GetTechnoType()->Ammo > 1)
				{
					int ammo = pTechno->Ammo;
					if (ammo >= 0)
					{
						pGift->Ammo = ammo;
					}
				}

				// 继承AE管理器
				AttachEffect* giftAEM = nullptr;
				if (inheritAE)
				{
					inheritAE = false;
					//TODO 继承除了GiftBox之外的状态机
					// InheritedStatsTo(giftStatus);
					// 将礼物盒的AE管理器脱离，并交换给礼物
					if (TryGetAEManager<TechnoExt>(pGift, giftAEM))
					{
						Component* giftGO = giftAEM->GetParent();

						AttachEffect* boxAEM = AEManager();
						Component* boxGO = boxAEM->GetParent();

						boxAEM->DetachFromParent(false);
						giftAEM->DetachFromParent(false);

						// TODO 将当前的GiftBox的AE关闭，如果有的话
						// 从状态机中获取生效的AE并关闭

						giftGO->AddComponent(boxAEM);
						boxGO->AddComponent(giftAEM);

						giftAEM = boxAEM;
					}
				}

				// 附加新的AE
				if (!data.AttachEffects.empty())
				{
					if (!giftAEM)
					{
						giftAEM = GetAEManager<TechnoExt>(pGift);
					}
					if (giftAEM)
					{
						giftAEM->Attach(data.AttachEffects, data.AttachChances);
					}
				}

				// 强制任务
				if (data.ForceMission != Mission::None && data.ForceMission != Mission::Move)
				{
					// 强制任务
					pGift->QueueMission(data.ForceMission, false);
				}
				else
				{
					if (pTarget && data.InheritTarget && CanAttack(pGift, pTarget))
					{
						// 同步目标
						pGift->SetTarget(pTarget);
						pGift->QueueMission(currentMission, false);
					}
					else
					{
						if (!pDest && !pFocus)
						{
							// 第一个傻站着，第二个之后的散开
							if (scatter || pGiftType->BalloonHover)
							{
								// 分散到所在的格子里
								CoordStruct scatterPos = CoordStruct::Empty;
								if (pPutCell)
								{
									scatterPos = pPutCell->GetCoordsWithBridge();
								}
								pGift->Scatter(scatterPos, true, false);
							}
							scatter = true;
						}
						else
						{
							// 开往预定目的地
							if (!giftStatus->IsBuilding())
							{
								CoordStruct des = !pDest ? putLocation : pDest->GetCoords();
								if (pFocus)
								{
									pGift->SetFocus(pFocus);
									if (giftStatus->IsUnit())
									{
										des = pFocus->GetCoords();
									}
								}
								if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(des))
								{
									pGift->SetDestination(pTargetCell, true);
									pGift->QueueMission(Mission::Move, true);
								}
							}
						}
					}
				}
			}
			else
			{
				Debug::Log("Warning: Gift box release gift failed, unknown TechnoType [%s]\n", id.c_str());
			}
		}
	}
}
