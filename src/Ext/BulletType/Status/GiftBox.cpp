#include "../BulletStatus.h"

#include <FootClass.h>
#include <JumpjetLocomotionClass.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/Gift.h>
#include <Ext/Helper/Scripts.h>

#include <Ext/ObjectType/AttachEffect.h>
#include <Ext/TechnoType/TechnoStatus.h>

void BulletStatus::OnUpdate_GiftBox()
{
	TechnoClass* pTechno = pBullet->Owner;
	if (pTechno)
	{
		// 记录盒子的状态
		GiftBox->IsSelected = pTechno->IsSelected;
		GiftBox->Group = pTechno->Group;
	}
	if (GiftBox->IsActive())
	{
		// 记录朝向
		GiftBox->BodyDir = Facing(pBullet);
		GiftBox->TurretDir = GiftBox->BodyDir;
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
				bool harmless = !GiftBox->Data.Destroy;
				life.Detonate(harmless);
			}
			else
			{
				GiftBox->ResetGiftBox();
			}
		}
	}

}

bool BulletStatus::OnDetonate_GiftBox(CoordStruct* pCoords)
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
	return false;
}

bool BulletStatus::IsOnMark_GiftBox()
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

void BulletStatus::ReleaseGift(std::vector<std::string> gifts, GiftBoxData data)
{
	HouseClass* pHouse = pSourceHouse;
	// AE ReceiverOwn
	if (!GiftBox->ReceiverOwn && GiftBox->pAEHouse != nullptr)
	{
		pHouse = GiftBox->pAEHouse;
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
				}

				// 同步选中状态
				if (GiftBox->IsSelected)
				{
					giftStatus->DisableSelectVoice = true;
					pGift->Select();
					giftStatus->DisableSelectVoice = false;
				}

				// 附加AE
				if (!data.AttachEffects.empty())
				{
					AttachEffect* giftAEM = GetAEManager<TechnoExt>(pGift);
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
