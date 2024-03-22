﻿#include "../TechnoStatus.h"

#include <FootClass.h>
#include <JumpjetLocomotionClass.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/EffectType/AttachEffectScript.h>
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
		if ((IsInfantry() && dynamic_cast<InfantryClass*>(pTechno)->SequenceAnim == Sequence::Deployed)
			|| (IsUnit() && dynamic_cast<UnitClass*>(pTechno)->Deployed))
		{
			// 步兵或载具部署完毕，开始变形
			GiftBox->Start(GetTransformData());
		}
	}
}

void TechnoStatus::OnUpdate_GiftBox()
{
	// 记录单位的状态
	if (GiftBox->IsAlive())
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
			FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
			GiftBox->BodyDir = dynamic_cast<JumpjetLocomotionClass*>(pFoot->Locomotor.get())->LocomotionFacing.Current();
			GiftBox->TurretDir = GiftBox->BodyDir;
		}

		// 准备开盒
		if (GiftBox->CanOpen() && IsOnMark_GiftBox() && !GiftBox->Data.OpenWhenDestroyed && !GiftBox->Data.OpenWhenHealthPercent)
		{
			// 开盒
			GiftBox->IsOpen = true;
			GiftBoxData data = GiftBox->Data;
			// 释放礼物
			std::vector<std::string> gifts = GetGiftList(GiftBox->GetGiftData());
			if (!gifts.empty())
			{
				ReleaseGift(gifts, data);
			}
		}

		// 重置或者销毁盒子
		if (GiftBox->IsOpen)
		{
			if (GiftBox->Data.Remove)
			{
				GiftBox->End();
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
				_isDead = true;
				// 重要，击杀自己后中断所有后续循环
				Break();
			}
			else
			{
				GiftBox->ResetGiftBox();
			}
		}
	}

}

void TechnoStatus::OnReceiveDamageEnd_GiftBox(int* pRealDamage, WarheadTypeClass* pWH, DamageState damageState, TechnoClass* pAttacker, HouseClass* pAttackingHouse)
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
			GiftBoxData data = GiftBox->Data;
			// 释放礼物
			std::vector<std::string> gifts = GetGiftList(GiftBox->GetGiftData());
			if (!gifts.empty())
			{
				ReleaseGift(gifts, data);
			}
			// 此处不重置或销毁，而是进入下一帧的Update事件中重置或销毁
		}
	}
}

void TechnoStatus::OnReceiveDamageDestroy_GiftBox()
{
	if (GiftBox->IsAlive() && GiftBox->Data.OpenWhenDestroyed && IsOnMark_GiftBox())
	{
		// 开盒
		GiftBox->IsOpen = true;
		GiftBoxData data = GiftBox->Data;
		// 释放礼物
		std::vector<std::string> gifts = GetGiftList(GiftBox->GetGiftData());
		if (!gifts.empty())
		{
			ReleaseGift(gifts, data);
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
				return CheckOnMarks(aeMarks, marks);
			}
		}
		return false;
	}
	return true;
}

void TechnoStatus::ReleaseGift(std::vector<std::string> gifts, GiftBoxData data)
{
	HouseClass* pHouse = pTechno->Owner;
	// AE ReceiverOwn
	if (!GiftBox->ReceiverOwn && GiftBox->pAEHouse != nullptr)
	{
		pHouse = GiftBox->pAEHouse;
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
	AbstractClass* pDest = nullptr; // 载具当前的移动目的地
	AbstractClass* pFocus = nullptr; // 步兵当前的移动目的地
	// 获取目的地
	if (!IsBuilding())
	{
		pDest = dynamic_cast<FootClass*>(pTechno)->Destination;
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
	bool inheritAE = data.Remove && data.InheritAE;

	// 读取记录信息
	BoxStateCache boxState = GiftBox->GetGiftBoxStateCache();
	boxState.Location = location;
	boxState.CurrentMission = currentMission;
	boxState.pTarget = pTarget;

	boxState.pDest = pDest;
	boxState.pFocus = pFocus;

	boxState.CrateBuff = CrateBuff;

	boxState.pOwner = pTechno;
	boxState.pHouse = pHouse;
	// 开刷
	ReleaseGifts(gifts, GiftBox->GetGiftData(), boxState,
		[&](TechnoClass* pGift, TechnoStatus*& pGiftStatus, AttachEffect*& pGiftAEM)
		{
			TechnoTypeClass* pGiftType = pGift->GetTechnoType();
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
			if (inheritAE)
			{
				inheritAE = false;
				// 复制除了giftBox之外的状态机
				InheritedStatsTo(pGiftStatus);
				// 获取根组件
				Component* giftGO = pGiftStatus->GetParent();
				Component* boxGO = this->GetParent();
				// 交换AE管理器
				AttachEffect* boxAEM = boxGO->GetComponent<AttachEffect>();
				// 关闭不可继承的AE，以及含有GiftBox的AE
				boxAEM->ForeachChild([&](Component* c) {
					if (auto ae = dynamic_cast<AttachEffectScript*>(c))
					{
						if (!ae->AEData.Inheritable || ae->AEData.GiftBox.Enable || ae->AEData.Transform.Enable)
						{
							ae->TimeToDie();
						}
						else if (ae->pSource == pTechno)
						{
							// 如果来源是盒子，继承时，来源修改为礼物
							ae->pSource = pGift;
							ae->pSourceHouse = boxState.pHouse;
						}
					}
					});
				boxAEM->CheckDurationAndDisable(true);
				boxAEM->ClearDisableComponent();
				// AE管理器脱离
				boxAEM->DetachFromParent(false);
				pGiftAEM->DetachFromParent(false);
				// 交换
				giftGO->AddComponent(boxAEM);
				boxGO->AddComponent(pGiftAEM);
				// 修改变量
				pGiftAEM = boxAEM;
				// 发出类型变更的通知
				boxAEM->ExtChanged();
				pGiftAEM->ExtChanged();
				dynamic_cast<GameObject*>(giftGO)->ExtChanged = true;
			}
		});
}

