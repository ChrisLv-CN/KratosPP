#pragma once

#include <string>
#include <vector>
#include <map>

#include <GeneralStructures.h>
#include <VoxelAnimTypeClass.h>
#include <VoxelAnimClass.h>

#include "ExpandAnimsData.h"

class ExpandAnimsManager
{
public:
	/**
	 *@brief 在指定位置刷出动画
	 *
	 * @param data 控制参数
	 * @param location 位置
	 * @param pHouse 所属
	 */
	static void PlayExpandAnims(ExpandAnimsData data, CoordStruct location, HouseClass* pHouse = nullptr);

	/**
	 *@brief 在指定位置刷出vxl碎片
	 *
	 * @param types vxl碎片列表，从YRPP中获得，如Warhead->DebrisTypes
	 * @param nums 每个碎片的数量
	 * @param times 次数
	 * @param location 位置
	 * @param pHouse 所属
	 * @param pCreater 所属
	 */
	static void PlayExpandDebirs(DynamicVectorClass<VoxelAnimTypeClass*> types, DynamicVectorClass<int> nums, int times, CoordStruct location, HouseClass* pHouse, TechnoClass* pCreater);
};
