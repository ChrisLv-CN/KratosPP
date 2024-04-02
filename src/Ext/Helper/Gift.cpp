﻿#include "Gift.h"
#include "MathEx.h"
#include "StringEx.h"
#include "Scripts.h"

#include <Unsorted.h>
#include <JumpjetLocomotionClass.h>
#include <FootClass.h>

#include <Utilities/Debug.h>

#include <Extension/TechnoExt.h>
#include <Ext/TechnoType/TechnoStatus.h>

#include <Ext/EffectType/Effect/CrateBuffData.h>

bool TryPutTechno(TechnoClass* pTechno, CoordStruct location, CellClass* pCell, bool virtualUnit)
{
	if (pCell || (pCell = MapClass::Instance->TryGetCellAt(location)) != nullptr)
	{
		auto occFlags = pCell->OccupationFlags;
		pTechno->OnBridge = pCell->ContainsBridge();
		CoordStruct xyz = pCell->GetCoordsWithBridge();
		++Unsorted::IKnowWhatImDoing;
		pTechno->Unlimbo(xyz, DirType::East);
		--Unsorted::IKnowWhatImDoing;
		if (virtualUnit)
		{
			pCell->OccupationFlags = occFlags;
		}
		bool dontMove = false;
		if (BuildingClass* pBuilding = dynamic_cast<BuildingClass*>(pTechno))
		{
			if (!virtualUnit && pBuilding->Type->Foundation != Foundation::_0x0)
			{
				pBuilding->QueueMission(Mission::Construction, false);
				pCell->AddContent(pBuilding, false);
				dontMove = true;
			}
			else
			{
				dontMove = false;
			}
			pBuilding->DiscoveredBy(pTechno->Owner);
			pBuilding->IsReadyToCommence = true;
			pTechno->Owner->RecheckTechTree = true;
		}
		if (!dontMove)
		{
			// 单位移动到指定位置
			xyz.Z = location.Z;
			pTechno->SetLocation(xyz);
		}
		return true;
	}
	return false;
}

TechnoClass* CreateAndPutTechno(TechnoTypeClass* pType, HouseClass* pHouse, CoordStruct location, CellClass* pCell)
{
	TechnoClass* pTechno = dynamic_cast<TechnoClass*>(pType->CreateObject(pHouse));
	if (TryPutTechno(pTechno, location, pCell))
	{
		return pTechno;
	}
	return nullptr;
}

TechnoClass* CreateAndPutTechno(std::string id, HouseClass* pHouse, CoordStruct location, CellClass* pCell)
{
	if (IsNotNone(id))
	{
		TechnoTypeClass* pType = TechnoTypeClass::Find(id.c_str());
		if (pType)
		{
			return CreateAndPutTechno(pType, pHouse, location, pCell);
		}
	}
	return nullptr;
}

bool RealReleaseGift(std::string id, HouseClass* pHouse,
	CoordStruct location, // 预订投放位置
	CellClass* pCell, // 预订投放位置的格子
	std::vector<CellStruct> cellOffsets, // 随机格子的范围
	bool emptyCell, // 是否空格子再投放
	TechnoTypeClass*& pGiftType, // 礼物的类型
	TechnoClass*& pGift, // 礼物本体
	CoordStruct& putLocation, // 礼物的投放位置
	CellClass*& pPutCell // 礼物投放的格子
)
{
	pGiftType = nullptr;
	pGift = nullptr;
	putLocation = location;
	pPutCell = pCell;
	CellStruct centerCellPos = pCell->MapCoords;
	if (IsNotNone(id) && (pGiftType = TechnoTypeClass::Find(id.c_str())) != nullptr)
	{
		int max = cellOffsets.size();
		for (int i = 0; i < max; i++)
		{
			int index = Random::RandomRanged(0, max - 1);
			CellStruct offset = cellOffsets[index];
			CellStruct targetCellPos = centerCellPos + offset;
			// 检查目标格子是否可用
			if (CellClass* pTargetCell = MapClass::Instance->TryGetCellAt(targetCellPos))
			{
				if (pTargetCell->IsClearToMove(pGiftType->SpeedType, pGiftType->MovementZone, !emptyCell, !emptyCell))
				{
					pPutCell = pTargetCell;
					putLocation = pTargetCell->GetCoordsWithBridge();
					break;
				}
			}
		}
		// 投送单位
		pGift = CreateAndPutTechno(pGiftType, pHouse, putLocation, pPutCell);
	}

	return pGift != nullptr;
}


BoxStateCache GiftBoxStateCache(GiftBoxData data)
{
	BoxStateCache boxState;
	// boxState.Location = pBullet->GetCoords();
	boxState.RandomRange = data.RandomRange;
	boxState.EmptyCell = data.EmptyCell;

	// boxState.CurrentMission = currentMission;
	boxState.ForceMission = data.ForceMission;
	// boxState.pTarget = pTarget;
	boxState.InheritTarget = data.InheritTarget;

	// boxState.pDest = pFocus;
	// boxState.pFocus = pFocus;
	boxState.Scatter = !data.Remove || data.ForceMission == Mission::Move;

	boxState.SameDir = data.IsTransform;
	// boxState.BodyDir = BodyDir;
	// boxState.TurretDir = TurretDir;

	// boxState.Group = Group;
	// boxState.IsSelected = IsSelected;
	// boxState.CrateBuff = CrateBuff;

	// boxState.pHouse = pSourceHouse;

	boxState.RemoveEffects = data.RemoveEffects;
	boxState.AttachEffects = data.AttachEffects;
	boxState.AttachChances = data.AttachChances;
	return boxState;
}

void ReleaseGifts(std::vector<std::string> gifts, GiftBoxEntity data, BoxStateCache boxState, std::function<void(TechnoClass*, TechnoStatus*&, AttachEffect*&)> inheritFun)
{
	CoordStruct location = boxState.Location;

	// 获取投送单位的位置
	if (CellClass* pCell = MapClass::Instance->TryGetCellAt(location))
	{
		HouseClass* pHouse = boxState.pHouse;
		Mission currentMission = boxState.CurrentMission;
		Mission forceMission = boxState.ForceMission;
		// 获取目的地
		AbstractClass* pDest = boxState.pDest; // 载具当前的移动目的地
		AbstractClass* pFocus = boxState.pFocus; // 步兵当前的移动目的地

		AbstractClass* pTarget = boxState.pTarget;
		bool scatter = boxState.Scatter;
		// 随机投送位置
		CellStruct cellPos = pCell->MapCoords;
		std::vector<CellStruct> cellOffsets{};
		if (boxState.RandomRange > 0)
		{
			auto const range = static_cast<size_t>(boxState.RandomRange + 0.99);
			for (CellSpreadEnumerator it(boxState.RandomRange); it; ++it) {
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
				location, pCell, cellOffsets, boxState.EmptyCell,
				pGiftType, pGift,
				putLocation, pPutCell)
				)
			{
				TechnoStatus* pGiftStatus = GetStatus<TechnoExt, TechnoStatus>(pGift);
				if (boxState.SameDir)
				{
					// 同步朝向
					pGift->PrimaryFacing.SetCurrent(boxState.BodyDir);
					pGift->SecondaryFacing.SetCurrent(boxState.TurretDir);
					// JJ有单独的Facing
					if (pGiftStatus->IsJumpjet())
					{
						FootClass* pGiftFoot = dynamic_cast<FootClass*>(pGift);
						JumpjetLocomotionClass* pLoco = dynamic_cast<JumpjetLocomotionClass*>(pGiftFoot->Locomotor.get());
						pLoco->LocomotionFacing.SetCurrent(boxState.BodyDir);
					}
					// 同步编队
					pGift->Group = boxState.Group;
					if (!boxState.BoxIsBullet) {
						// 同步箱子属性
						pGiftStatus->CrateBuff = boxState.CrateBuff;
					}
				}

				// 同步选中状态
				if (boxState.IsSelected)
				{
					pGiftStatus->DisableSelectVoice = true;
					pGift->Select();
					pGiftStatus->DisableSelectVoice = false;
				}

				AttachEffect* giftAEM = GetAEManager<TechnoExt>(pGift);
				// 调用继承
				inheritFun(pGift, pGiftStatus, giftAEM);

				// 调整AE
				if (giftAEM)
				{
					// 移除失效的AE
					giftAEM->DetachByName(boxState.RemoveEffects);
					// 附加新的AE
					giftAEM->Attach(boxState.AttachEffects, boxState.AttachChances, false, boxState.pOwner, pHouse);
				}

				// 强制任务
				if (forceMission != Mission::None && forceMission != Mission::Move)
				{
					// 强制任务
					pGift->QueueMission(forceMission, false);
				}
				else
				{
					if (pTarget && boxState.InheritTarget && CanAttack(pGift, pTarget))
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
							if (!pGiftStatus->IsBuilding())
							{
								CoordStruct des = !pDest ? putLocation : pDest->GetCoords();
								if (pFocus)
								{
									pGift->SetFocus(pFocus);
									if (pGiftStatus->IsUnit())
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

std::vector<std::string> GetGiftList(GiftBoxEntity data)
{
	std::vector<std::string> gifts{};
	if (data.Enable)
	{
		int giftCount = data.Gifts.size();
		int numsCount = data.Nums.size();
		if (data.RandomType)
		{
			// 随机类型，将所有的数量限制加起来，总和为礼物数量
			int times = 1;
			if (numsCount > 0)
			{
				times = 0;
				for (int num : data.Nums)
				{
					times += num;
				}
			}
			// 计算权重
			int maxValue = 0;
			std::map<Point2D, int> targetPad = MakeTargetPad(data.RandomWeights, giftCount, maxValue);
			// 算出随机值，确认位置，取得序号，选出单位
			for (int i = 0; i < times; i++)
			{
				int index = Hit(targetPad, maxValue);
				// 计算概率
				if (Bingo(data.Chances, index))
				{
					std::string id = data.Gifts[index];
					if (IsNotNone(id))
					{
						gifts.emplace_back(id);
					}
				}
			}
		}
		else
		{
			// 指定类型，遍历礼物类型，取指定数量个
			for (int index = 0; index < giftCount; index++)
			{
				std::string id = data.Gifts[index];
				if (IsNotNone(id))
				{
					int times = 1;
					if (numsCount > 0 && index < numsCount)
					{
						times = data.Nums[index];
					}
					// 取指定数量个
					for (int i = 0; i < times; i++)
					{
						// 计算概率
						if (Bingo(data.Chances, index))
						{
							gifts.push_back(id); // 存copy
						}
					}
				}
			}
		}
	}
	return gifts;
}

