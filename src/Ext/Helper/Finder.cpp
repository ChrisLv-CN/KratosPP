#include "Finder.h"
#include "CastEx.h"
#include "MathEx.h"
#include "Status.h"

#include <string>
#include <vector>

#include <Utilities/Debug.h>

TechnoClass* FindRandomTechno(HouseClass* pHouse)
{
	// 遍历所有单位，组成这个所属的单位清单
	DynamicVectorClass<TechnoClass*>* pArray = TechnoClass::Array.get();
	std::vector<TechnoClass*> pTechnoArray{};
	for (auto it = pArray->begin(); it != pArray->end(); it++)
	{
		TechnoClass* pTechno = *it;
		if (pTechno && pTechno->Owner && pTechno->Owner == pHouse)
		{
			pTechnoArray.push_back(pTechno);
		}
	}
	// 抽取一个幸运儿
	int size = pTechnoArray.size();
	if (size > 0)
	{
		int targetIdx = Random::RandomRanged(0, size - 1); // 同一帧的随机值永远都是同一个，容易造成死循环
		bool forward = true;
		for (int i = 0; i < size; i++)
		{
			TechnoClass* pTarget = pTechnoArray[targetIdx];
			if (!IsDeadOrInvisible(pTarget))
			{
#ifdef DEBUG
				Debug::Log("Pickup a Luckey Techno [%s]%d\n", pTarget->GetType()->ID, pTarget);
#endif // DEBUG
				return pTarget;
			}
			if (targetIdx >= size - 1)
			{
				targetIdx = size - 1;
				forward = false;
			}
			else if (targetIdx <= 0)
			{
				targetIdx = 0;
				forward = true;
			}
			if (forward)
			{
				targetIdx++;
			}
			else
			{
				targetIdx--;
			}
		}
	}
	return nullptr;
}

double Finder::DistanceFrom(CoordStruct sourcePos, CoordStruct targetPos, bool fullAirspace)
{
	if (fullAirspace)
	{
		CoordStruct tempSource = sourcePos;
		CoordStruct tempTarget = targetPos;
		tempSource.Z = 0;
		tempTarget.Z = 0;
		return tempSource.DistanceFrom(tempTarget);
	}
	return sourcePos.DistanceFrom(targetPos);
}

bool Finder::InRange(CoordStruct location, CoordStruct targetLocation,
	double maxRange, double minRange, bool fullAirspace,
	bool isInAir, AbstractType abstractType)
{
	double distance = DistanceFrom(location, targetLocation, fullAirspace);
	if (!fullAirspace && isInAir && abstractType == AbstractType::Aircraft)
	{
		distance *= 0.5;
	}
	return distance >= minRange && distance <= maxRange;
}

bool Finder::Hit(ObjectClass* pObject,
	CoordStruct location, double maxRange, double minRange, bool fullAirspace,
	HouseClass* pHouse,
	bool owner, bool allied, bool enemies, bool civilian)
{
	bool inRange = maxRange == 0 || location.IsEmpty();
	CoordStruct targetLocation = inRange ? CoordStruct::Empty : pObject->GetCoords(); // 不检查距离就不用算
	bool isInAir = false; // 全空域就不用算
	AbstractType abstractType = AbstractType::None; // 全空域就不用算
	if (!fullAirspace)
	{
		isInAir = pObject->IsInAir();
		abstractType = pObject->WhatAmI();
	}
	if (inRange || InRange(location, targetLocation, maxRange, minRange, fullAirspace, isInAir, abstractType))
	{
		TechnoClass* pTechno = nullptr;
		BulletClass* pBullet = nullptr;
		if (CastToTechno(pObject, pTechno))
		{
			HouseClass* pTargetHouse = pTechno->Owner;
			return CanAffectHouse(pHouse, pTargetHouse, owner, allied, enemies, civilian);
		}
		else if (CastToBullet(pObject, pBullet) && !IsDeadOrInvisible(pBullet))
		{
			HouseClass* pTargetHouse = GetHouse(pBullet);
			if (pHouse && pTargetHouse)
			{
				// 检查原始所属
				return CanAffectHouse(pHouse, pTargetHouse, owner, allied, enemies, civilian);
			}
		}
		else
		{
			return false;
		}
		return true;
	}
	return false;
}
