#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>
#include <HouseClass.h>
#include <MapClass.h>
#include <TechnoClass.h>
#include <TechnoTypeClass.h>

#include <Helpers/Enumerators.h>

TechnoClass* CreateAndPutTechno(TechnoTypeClass* pType, HouseClass* pHouse, CoordStruct location, CellClass* pCell = nullptr);

TechnoClass* CreateAndPutTechno(const char* id, HouseClass* pHouse, CoordStruct location, CellClass* pCell = nullptr);

bool RealReleaseGift(std::string id, HouseClass* pHouse,
	CoordStruct location, // 预订投放位置
	CellClass* pCell, // 预订投放位置的格子
	std::vector<CellStruct> cellOffsets, // 随机格子的范围
	bool emptyCell, // 是否空格子再投放
	TechnoTypeClass*& pGiftType, // 礼物的类型
	TechnoClass*& pGift, // 礼物本体
	CoordStruct& putLocation, // 礼物的投放位置
	CellClass*& pPutCell // 礼物投放的格子
);
