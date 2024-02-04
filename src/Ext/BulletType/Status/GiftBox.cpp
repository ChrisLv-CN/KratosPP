#include "../BulletStatus.h"

#include <FootClass.h>
#include <JumpjetLocomotionClass.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/TechnoType/TechnoStatus.h>

void BulletStatus::InitState_GiftBox()
{
	// 读取单位身上的礼盒设置
	GiftBoxData* data = INI::GetConfig<GiftBoxData>(INI::Rules, pBullet->Type->ID)->Data;
	if (data->Enable)
	{
		GiftBoxState.Enable(*data);
	}
}

void BulletStatus::OnUpdate_GiftBox()
{
	TechnoClass* pTechno = pBullet->Owner;
	if (pTechno)
	{
		GiftBoxState.Update(pTechno->Veterancy.IsElite());
		// 记录盒子的状态
		GiftBoxState.IsSelected = pTechno->IsSelected;
		GiftBoxState.Group = pTechno->Group;
	}
	if (GiftBoxState.IsActive())
	{
		// 记录朝向
		GiftBoxState.BodyDir = Facing(pBullet);
		GiftBoxState.TurretDir = GiftBoxState.BodyDir;
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
				bool harmless = !GiftBoxState.Data.Destroy;
				life.Detonate(harmless);
			}
			else
			{
				GiftBoxState.ResetGiftBox();
			}
		}
	}

}

bool BulletStatus::OnDetonate_GiftBox(CoordStruct* pCoords)
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
	return false;
}

bool BulletStatus::IsOnMark_GiftBox()
{
	// TODO AE isOnMark
	return true;
}

void BulletStatus::ReleaseGift(std::vector<std::string> gifts, GiftBoxData data)
{
	HouseClass* pHouse = pSourceHouse;
	// TODO AE ReceiverOwn
	if (false)
	{

	}
	CoordStruct location = pBullet->GetCoords();
	// 获取投送单位的位置
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		AbstractClass* pTarget = pBullet->Target;
		AbstractClass* pFocus = nullptr; // 步兵的移动目的地

		// 取目标所在位置
		CoordStruct targetLocation = pBullet->TargetCoords;
		if (pTarget)
		{
			targetLocation = pTarget->GetCoords();
		}
		// 取目标位置所在的格子作为移动目的地
		CellClass* pTargetLocationCell = MapClass::Instance->TryGetCellAt(targetLocation);
		if (pTargetLocationCell)
		{
			pFocus = pTargetLocationCell; // 步兵的移动目的地
		}

		bool scatter = !data.Remove || data.ForceMission == Mission::Move;
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
				}

				// 同步选中状态
				if (GiftBoxState.IsSelected)
				{
					giftStatus->DisableSelectVoice = true;
					pGift->Select();
					giftStatus->DisableSelectVoice = false;
				}

				// 继承AE管理器
				/* TODO AE
				AttachEffectScript giftAEM = null;
				// 附加AE
				if (null != data.AttachEffects)
				{
					if (null == giftAEM)
					{
						giftAEM = pGift.GetAEManegr();
					}
					if (null != giftAEM)
					{
						giftAEM.Attach(data.AttachEffects, data.AttachChances);
					}
				}*/

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
						pGift->QueueMission(Mission::Attack, false);
					}
					else
					{
						if (!pFocus)
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
								if (pFocus)
								{
									pGift->SetFocus(pFocus);
									if (giftStatus->IsUnit())
									{
										pGift->SetDestination(pFocus, true);
										pGift->QueueMission(Mission::Move, true);
									}
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
