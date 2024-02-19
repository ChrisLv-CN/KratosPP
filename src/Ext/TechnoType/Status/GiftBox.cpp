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

void TechnoStatus::OnUpdate_DeployToTransform()
{
	if (GetTransformData()->Enable)
	{
		if ((IsInfantry() && static_cast<InfantryClass*>(pTechno)->SequenceAnim == Sequence::Deployed)
			|| (IsUnit() && static_cast<UnitClass*>(pTechno)->Deployed))
		{
			// 步兵或载具部署完毕，开始变形
			GiftBox->Start(GetTransformData());
		}
	}
}

void TechnoStatus::OnUpdate_GiftBox()
{
	// 记录单位的状态
	if (GiftBox->IsActive())
	{
		// 记录盒子的状态
		GiftBox->IsSelected = pTechno->IsSelected;
		GiftBox->Group = pTechno->Group;
		// 记录朝向
		GiftBox->BodyDir = pTechno->PrimaryFacing.Current();
		GiftBox->TurretDir = pTechno->SecondaryFacing.Current();
		// JJ有单独的Facing
		if (IsJumpjet())
		{
			FootClass* pFoot = static_cast<FootClass*>(pTechno);
			GiftBox->BodyDir = static_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get())->LocomotionFacing.Current();
			GiftBox->TurretDir = GiftBox->BodyDir;
		}

		// 准备开盒
		if (GiftBox->CanOpen() && IsOnMark_GiftBox() && !GiftBox->Data.OpenWhenDestroyed && !GiftBox->Data.OpenWhenHealthPercent)
		{
			// 开盒
			GiftBox->IsOpen = true;
			// 释放礼物
			std::vector<std::string> gifts = GiftBox->GetGiftList();
			if (!gifts.empty())
			{
				ReleaseGift(gifts, GiftBox->Data);
			}
		}

		// 重置或者销毁盒子
		if (GiftBox->IsOpen)
		{
			if (GiftBox->Data.Remove)
			{
				GiftBox->Disable();
				if (GiftBox->Data.Destroy)
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
				GiftBox->ResetGiftBox();
			}
		}
	}

}

void TechnoStatus::OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	if (damageState != DamageState::NowDead && IsDeadOrInvisible(pTechno)
		&& GiftBox->CanOpen() && GiftBox->Data.OpenWhenHealthPercent
		&& IsOnMark_GiftBox()
		)
	{
		// 计算血量百分比是否达到开启条件
		double healthPercent = pTechno->GetHealthPercentage();
		if (healthPercent <= GiftBox->Data.OpenHealthPercent)
		{
			// 开盒
			GiftBox->IsOpen = true;
			// 释放礼物
			std::vector<std::string> gifts = GiftBox->GetGiftList();
			if (!gifts.empty())
			{
				ReleaseGift(gifts, GiftBox->Data);
			}
			// 此处不重置或销毁，而是进入下一帧的Update事件中重置或销毁
		}
	}
}

void TechnoStatus::OnReceiveDamageDestroy_GiftBox()
{
	if (GiftBox->IsActive() && GiftBox->Data.OpenWhenDestroyed && IsOnMark_GiftBox())
	{
		// 开盒
		GiftBox->IsOpen = true;
		// 释放礼物
		std::vector<std::string> gifts = GiftBox->GetGiftList();
		if (!gifts.empty())
		{
			ReleaseGift(gifts, GiftBox->Data);
		}
	}
}

bool TechnoStatus::IsOnMark_GiftBox()
{
	std::vector<std::string> marks = GiftBox->Data.OnlyOpenWhenMarks;
	if (!marks.empty())
	{
		if (AttachEffect* aem = AEManager())
		{
			std::vector<std::string> aeMarks{};
			aem->GetMarks(aeMarks);
			if (!aeMarks.empty())
			{
				// 取交集
				std::set<std::string> m(marks.begin(), marks.end());
				std::set<std::string> t(aeMarks.begin(), aeMarks.end());
				std::set<std::string> v;
				std::set_intersection(m.begin(), m.end(), t.begin(), t.end(), std::inserter(v, v.begin()));
				return !v.empty();
			}
		}
		return false;
	}
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
					pGift->PrimaryFacing.SetCurrent(GiftBox->BodyDir);
					pGift->SecondaryFacing.SetCurrent(GiftBox->TurretDir);
					// JJ有单独的Facing
					if (giftStatus->IsJumpjet())
					{
						FootClass* pGiftFoot = static_cast<FootClass*>(pGift);
						JumpjetLocomotionClass* pLoco = static_cast<JumpjetLocomotionClass*>(pGiftFoot->Locomotor.get());
						pLoco->LocomotionFacing.SetCurrent(GiftBox->BodyDir);
					}
					// 同步编队
					pGift->Group = GiftBox->Group;
					// 同步箱子属性
					giftStatus->CrateBuff = CrateBuff;
				}

				// 同步选中状态
				if (GiftBox->IsSelected)
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
					// 复制除了giftBox之外的状态机
					InheritedStatsTo(giftStatus);
					// 获取根组件
					Component* giftGO = giftStatus->GetParent();
					Component* boxGO = this->GetParent();
					// 交换AE管理器
					giftAEM = giftGO->GetComponent<AttachEffect>();
					AttachEffect* boxAEM = boxGO->GetComponent<AttachEffect>();
					// 将当前的GiftBox的AE关闭，如果有的话
					boxAEM->DetachByToken(GiftBox->Token);
					// AE管理器脱离
					boxAEM->DetachFromParent(false);
					giftAEM->DetachFromParent(false);
					// 交换
					giftGO->AddComponent(boxAEM);
					boxGO->AddComponent(giftAEM);
					// 修改变量
					giftAEM = boxAEM;
					// 发出类型变更的通知
					dynamic_cast<GameObject*>(giftGO)->ExtChanged = true;
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

void TechnoStatus::InheritedStatsTo(TechnoStatus*& heir)
{
	*heir->AntiBullet = *AntiBullet;
	*heir->DestroyAnim = *DestroyAnim;
	*heir->DestroySelf = *DestroySelf;
	*heir->FireSuper = *FireSuper;
	// *heir->GiftBox = *this->GiftBox;
	*heir->Paintball = *Paintball;
	*heir->Transform = *Transform;
	// 状态机
	// GET_STATE(AntiBullet);
	// GET_STATE(DestroyAnim);
	// GET_STATE(DestroySelf);
	// GET_STATE(FireSuper);
	// GET_STATE(GiftBox);
	// GET_STATE(Paintball);
	// GET_STATE(Transform);
}
