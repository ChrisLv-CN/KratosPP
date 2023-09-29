#include "Gift.h"
#include "MathEx.h"
#include "StringEx.h"

#include <Unsorted.h>

TechnoClass* CreateAndPutTechno(TechnoTypeClass* pType, HouseClass* pHouse, CoordStruct location, CellClass* pCell)
{
	TechnoClass* pTechno = static_cast<TechnoClass*>(pType->CreateObject(pHouse));
	if (!pCell || (pCell = MapClass::Instance->TryGetCellAt(location)) != nullptr)
	{
		auto occFlags = pCell->OccupationFlags;
		pTechno->OnBridge = pCell->ContainsBridge();
		++Unsorted::IKnowWhatImDoing;
		pTechno->Unlimbo(pCell->GetCoordsWithBridge(), DirType::East);
		--Unsorted::IKnowWhatImDoing;
		pCell->OccupationFlags = occFlags;
		// 单位移动到指定位置
		pTechno->SetLocation(location);
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
			int index = GetRandom().RandomRanged(0, max - 1);
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
