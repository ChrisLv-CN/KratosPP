#pragma once

#include <cmath>
#include <functional>
#include <string>

#include <GeneralStructures.h>
#include <HouseClass.h>
#include <ObjectClass.h>
#include <BulletClass.h>
#include <TechnoClass.h>

#include <Ext/Helper/CastEx.h>
#include <Ext/Helper/FLH.h>

TechnoClass* FindRandomTechno(HouseClass* pHouse);

namespace Finder
{
	/**
	 *@brief 计算两点之间的距离
	 *
	 * @param sourcePos 起始位置
	 * @param targetPos 终点位置
	 * @param fullAirspace 无视高度
	 * @return double
	 */
	double DistanceFrom(CoordStruct sourcePos, CoordStruct targetPos, bool fullAirspace);

	/**
	 *@brief 检查目标位置是否在指定位置的范围内
	 *
	 * @param location 中心位置
	 * @param targetLocation 目标位置
	 * @param maxRange 最大范围
	 * @param minRange 最小范围
	 * @param fullAirspace 全空域检查
	 * @param isInAir 目标是否在天上
	 * @param abstractType 目标类型
	 * @return true
	 * @return false
	 */
	bool InRange(CoordStruct location, CoordStruct targetLocation,
		double maxRange, double minRange, bool fullAirspace,
		bool isInAir, AbstractType abstractType);

	/**
	 *@brief 检查目标对象是否在指定位置的范围内
	 *
	 * @param pObject 目标对象
	 * @param location 中心
	 * @param maxRange 最大范围
	 * @param minRange 最小范围
	 * @param fullAirspace 全空域检查
	 * @param pHouse 比对所属阵营
	 * @param owner 自己人
	 * @param allied 友军
	 * @param enemies 敌军
	 * @param civilian 平民
	 * @return true
	 * @return false
	 */
	bool Hit(ObjectClass* pObject,
		CoordStruct location, double maxRange, double minRange, bool fullAirspace,
		HouseClass* pHouse,
		bool owner, bool allied, bool enemies, bool civilian);
}

/**
 *@brief 从列表中检索一个符合条件的对象，目标对象为ObjectClass的派生
 *
 * @tparam TBase 目标类型
 * @param array 列表
 * @param func 查找到后的回调函数
 * @param location 查找的位置
 * @param maxSpread 最大范围
 * @param minSpread 最小范围
 * @param fullAirspace 忽略高度，圆柱形判定
 * @param pHouse 比较的阵营
 * @param owner 包括自己人
 * @param allied 包括友军
 * @param enemies 包括敌军
 * @param civilian 包括平民
 */
template <typename TBase>
void FindObject(DynamicVectorClass<TBase*>* array, std::function<bool(TBase*)> func,
	CoordStruct location, double maxSpread, double minSpread = 0, bool fullAirspace = false,
	HouseClass* pHouse = nullptr,
	bool owner = true, bool allied = true, bool enemies = true, bool civilian = true)
{
	// 最大搜索范围小于0，搜索全部，等于0，搜索1格范围
	double maxRange = (maxSpread > 0 ? maxSpread : (maxSpread == 0 ? 1 : 0)) * 256;
	double minRange = (minSpread <= 0 ? 0 : minSpread) * 256;
	for (int i = array->Count - 1; i >= 0; i--)
	{
		TBase* pBase = array->GetItem(i);
		// 分离类型
		ObjectClass* pObject = static_cast<ObjectClass*>(pBase);
		if (pObject && Finder::Hit(pObject, location, maxRange, minRange, fullAirspace, pHouse, owner, allied, enemies, civilian))
		{
			if (func(pBase))
			{
				break;
			}
		}
	}
}

template <typename TBase>
void FindObject(DynamicVectorClass<TBase*>* array, std::function<void(TBase*)> func,
	HouseClass* pHouse = nullptr,
	bool owner = true, bool allied = true, bool enemies = true, bool civilian = true)
{
	FindObject<TBase>(array, func, CoordStruct::Empty, 0, 0, pHouse, owner, allied, enemies, civilian);
}
