#include "Finder.h"

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <TechnoClass.h>
#include <BuildingClass.h>
#include <InfantryClass.h>
#include <UnitClass.h>
#include <AircraftClass.h>
#include <BulletClass.h>

#include <Utilities/Debug.h>

#include <Extension/TechnoExt.h>
#include <Extension/WarheadTypeExt.h>

#include "CastEx.h"
#include "MathEx.h"
#include "Scripts.h"
#include "Status.h"

#include <Ext/BulletType/BulletStatus.h>
#include <Ext/ObjectType/AttachEffect.h>
#include <Ext/EffectType/AttachEffectData.h>
#include <Ext/EffectType/AttachEffectTypeData.h>
#include <Ext/TechnoType/TechnoStatus.h>


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
	return !isinf(distance) && distance >= minRange && distance <= maxRange;
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

std::vector<TechnoClass*> GetCellSpreadTechnos(CoordStruct location, double spread, bool fullAirspace, bool includeInAir, bool ignoreBuildingOuter,
	HouseClass* pHouse,
	bool owner, bool allied, bool enemies, bool civilian)
{
	std::set<TechnoClass*> pTechnoSet;

	// the quick way. only look at stuff residing on the very cells we are affecting.
	CellStruct cellCoords = MapClass::Instance->GetCellAt(location)->MapCoords;
	size_t range = static_cast<size_t>(spread + 0.99);
	for (CellSpreadEnumerator it(range); it; ++it) {
		CellClass* pCell = MapClass::Instance->GetCellAt(*it + cellCoords);
		// find all Techno in cell
		for (NextObject obj(pCell->GetContent()); obj; ++obj) {
			if (TechnoClass* pTechno = abstract_cast<TechnoClass*>(*obj)) {
				pTechnoSet.insert(pTechno);
			}
		}
		// Get JJ
		if (includeInAir && pCell && pCell->Jumpjet)
		{
			if (TechnoClass* pTechno = abstract_cast<TechnoClass*>(pCell->Jumpjet)) {
				pTechnoSet.insert(pTechno);
			}
		}
	}

	// flying objects are not included normally
	if (includeInAir) {
		// the not quite so fast way. skip everything not in the air.
		for (FootClass*& pTechno : *FootClass::Array) {
			if (pTechno->GetHeight() > 0) {
				// rough estimation
				if (pTechno->Location.DistanceFrom(location) <= spread * Unsorted::LeptonsPerCell) {
					pTechnoSet.insert(pTechno);
				}
			}
		}
	}

	// 筛选并去掉不可用项目
	std::vector<TechnoClass*> pTechnoList;
	for (TechnoClass* pTechno : pTechnoSet)
	{
		CoordStruct targetPos = pTechno->GetCoords();
		double dist = Finder::DistanceFrom(targetPos, location, fullAirspace);

		bool checkDistance = true;
		AbstractType absType = pTechno->WhatAmI();
		switch (absType)
		{
		case AbstractType::Building:
		{
			BuildingClass* pBuilding = static_cast<BuildingClass*>(pTechno);
			if (pBuilding->Type->InvisibleInGame) {
				continue;
			}
			if (!ignoreBuildingOuter)
			{
				checkDistance = false;
			}
			break;
		}
		case AbstractType::Aircraft:
			if (pTechno->IsInAir())
			{
				dist *= 0.5;
			}
			break;
		}

		if (!checkDistance || dist <= spread * Unsorted::LeptonsPerCell)
		{
			pTechnoList.push_back(pTechno);
		}
	}
	return pTechnoList;
}
void FindTechnoOnMark(std::function<void(TechnoClass*, AttachEffect*)> func,
	CoordStruct location, double maxSpread, double minSpread, bool fullAirspace,
	HouseClass* pHouse, FilterData data, ObjectClass* exclude)
{
	std::vector<TechnoClass*> pTechnoList;
	if (maxSpread <= 0)
	{
		// 搜索全部单位
		std::set<TechnoClass*> pTechnoSet;
		if (data.AffectBuilding)
		{
			FindAllObject<BuildingClass>(BuildingClass::Array.get(), [&](BuildingClass* pTarget)->bool {
				if (!pTarget->Type->InvisibleInGame) pTechnoSet.insert(pTarget); return false;
				}, pHouse, data.AffectsOwner, data.AffectsAllies, data.AffectsEnemies, data.AffectsCivilian);
		}
		if (data.AffectInfantry)
		{
			FindAllObject<InfantryClass>(InfantryClass::Array.get(), [&](InfantryClass* pTarget)->bool {
				if (data.AffectInAir || !pTarget->IsInAir()) pTechnoSet.insert(pTarget); return false;
				}, pHouse, data.AffectsOwner, data.AffectsAllies, data.AffectsEnemies, data.AffectsCivilian);
		}
		if (data.AffectUnit)
		{
			FindAllObject<UnitClass>(UnitClass::Array.get(), [&](UnitClass* pTarget)->bool {
				if (data.AffectInAir || !pTarget->IsInAir()) pTechnoSet.insert(pTarget); return false;
				}, pHouse, data.AffectsOwner, data.AffectsAllies, data.AffectsEnemies, data.AffectsCivilian);
		}
		if (data.AffectAircraft)
		{
			FindAllObject<AircraftClass>(AircraftClass::Array.get(), [&](AircraftClass* pTarget)->bool {
				if (data.AffectInAir || !pTarget->IsInAir()) pTechnoSet.insert(pTarget); return false;
				}, pHouse, data.AffectsOwner, data.AffectsAllies, data.AffectsEnemies, data.AffectsCivilian);
		}
		pTechnoList.assign(pTechnoSet.begin(), pTechnoSet.end());
	}
	else
	{
		// 小范围搜索
		pTechnoList = GetCellSpreadTechnos(location, maxSpread, fullAirspace, data.AffectInAir, true, pHouse, data.AffectsOwner, data.AffectsAllies, data.AffectsEnemies, data.AffectsCivilian);
	}
	// 去除不符合的目标
	for (TechnoClass* pTarget : pTechnoList)
	{
		if (IsDeadOrInvisible(pTarget) || (!data.AffectSelf && pTarget == exclude))
		{
			continue;
		}
		// 去除替身和虚单位
		TechnoStatus* status = nullptr;
		if (!data.AffectStand && TryGetStatus<TechnoExt>(pTarget, status) && (status->AmIStand() || status->VirtualUnit))
		{
			continue;
		}
		// 检查最小距离
		if (minSpread > 0)
		{
			double distance = Finder::DistanceFrom(pTarget->GetCoords(), location, fullAirspace);
			if (!fullAirspace && pTarget->IsInAir() && pTarget->WhatAmI() == AbstractType::Aircraft)
			{
				distance *= 0.5;
			}
			if (distance < minSpread * Unsorted::LeptonsPerCell)
			{
				continue;
			}
		}
		// 可影响
		AttachEffect* aeManager = nullptr;
		if (data.CanAffectType(pTarget) && TryGetAEManager<TechnoExt>(pTarget, aeManager) && data.OnMark(aeManager->GetMarks()))
		{
			// 执行动作
			func(pTarget, aeManager);
		}
	}
}


void FindBulletOnMark(std::function<void(BulletClass*, AttachEffect*)> func,
	CoordStruct location, double maxSpread, double minSpread, bool fullAirspace,
	HouseClass* pHouse, FilterData data, ObjectClass* exclude)
{
	std::set<BulletClass*> pBulletSet;
	FindObject<BulletClass>(BulletClass::Array.get(), [&](BulletClass* pTarget)->bool {
		if (IsDeadOrInvisible(pTarget) && (data.AffectSelf || pTarget != exclude) && data.CanAffectType(pTarget)) pBulletSet.insert(pTarget); return false;
		}, location, maxSpread, minSpread, fullAirspace, pHouse, data.AffectsOwner, data.AffectsAllies, data.AffectsEnemies, data.AffectsCivilian);
	// 去除不符合的目标
	for (BulletClass* pTarget : pBulletSet)
	{
		AttachEffect* aeManager = nullptr;
		if (TryGetAEManager<BulletExt>(pTarget, aeManager))
		{
			func(pTarget, aeManager);
		}
	}
}


void FindAndAttachEffect(CoordStruct location, int damage, WarheadTypeClass* pWH, ObjectClass* pAttacker, HouseClass* pAttackingHouse)
{
	AttachEffectTypeData* aeTypeData = GetAEData<WarheadTypeExt>(pWH);
	if (aeTypeData->Enable)
	{
		bool fullAirspace = aeTypeData->AttachFullAirspace;
		bool findTechno = false;
		bool findBullet = false;
		// 快速检索是否需要查找单位或者抛射体清单
		for (std::string ae : aeTypeData->AttachEffectTypes)
		{
			AttachEffectData* aeData = INI::GetConfig<AttachEffectData>(INI::Rules, ae.c_str())->Data;
			findTechno |= aeData->AffectTechno;
			findBullet |= aeData->AffectBullet;
		}

		WarheadTypeExt::TypeData* warheadTypeData = GetTypeData<WarheadTypeExt, WarheadTypeExt::TypeData>(pWH);
		if (findTechno)
		{
			double cellSpread = pWH->CellSpread;
			bool affectInAir = warheadTypeData->AffectInAir;
			// 检索爆炸范围内的单位类型
			std::vector<TechnoClass*> pTechnoList = GetCellSpreadTechnos(location, cellSpread, fullAirspace, affectInAir, false);
			// 排序准备和替身的并集
			std::sort(pTechnoList.begin(), pTechnoList.end());
			std::set<TechnoClass*> pTargetList;
			// 检索爆炸范围内的替身
			if (warheadTypeData->AffectStand)
			{
				// 检索爆炸范围内的替身
				std::vector<TechnoClass*> pStandArray;
				for (auto standExt : TechnoExt::StandArray)
				{
					pStandArray.push_back(standExt.first);
				}
				std::set<TechnoClass*> pStandList;
				// 过滤掉不在范围内的
				FindObject<TechnoClass>(pStandArray, [&pStandList, &affectInAir](TechnoClass* pTarget)->bool
					{
						if (affectInAir || !pTarget->IsInAir())
						{
							pStandList.insert(pTarget);
						}
						return false;
					}, location, (double)pWH->CellSpread);
				// 合并搜索到的单位和替身清单并去重
				std::set_union(pTechnoList.begin(), pTechnoList.end(), pStandList.begin(), pStandList.end(), std::inserter(pTargetList, pTargetList.begin()));
			}
			else
			{
				pTargetList.insert(pTechnoList.begin(), pTechnoList.end());
			}
			// Logger.Log($"{Game.CurrentFrame} 弹头[{pWH->Base.ID}] {pWH} 爆炸半径{pWH->CellSpread}, 影响的单位{pTechnoList.Count()}个，附加AE [{string.Join(", ", aeTypeData.AttachEffectTypes)}]");
			for (TechnoClass* pTarget : pTargetList)
			{
				// 检查死亡，过滤掉发射者
				if (IsDeadOrInvisible(pTarget) || (!warheadTypeData->AffectShooter && pTarget == pAttacker))
				{
					continue;
				}
				// 过滤替身和虚单位
				TechnoStatus* status = nullptr;
				if (!warheadTypeData->AffectStand && TryGetStatus<TechnoExt>(pTarget, status) && (status->AmIStand() || status->VirtualUnit))
				{
					continue;
				}
				int distanceFromEpicenter = (int)location.DistanceFrom(pTarget->GetCoords());
				HouseClass* pTargetHouse = pTarget->Owner;
				// 可影响可伤害
				int realDamage = 0;
				if (CanAffectHouse(pAttackingHouse, pTargetHouse, warheadTypeData->AffectsOwner, warheadTypeData->AffectsAllies, warheadTypeData->AffectsEnemies)// 检查所属权限
					&& CanDamageMe(pTarget, damage, (int)distanceFromEpicenter, pWH, realDamage)// 检查护甲
					)
				{
					// 赋予AE
					AttachEffect* aeManager = nullptr;
					if (TryGetAEManager<TechnoExt>(pTarget, aeManager))
					{
						aeManager->Attach(aeTypeData->AttachEffectTypes, aeTypeData->AttachEffectChances, false, pAttacker, pAttackingHouse, location);
					}
				}
			}
		}

		// 检索爆炸范围内的抛射体类型
		if (findBullet)
		{
			FindObject<BulletClass>(BulletClass::Array.get(), [&](BulletClass* pTarget)->bool {
				if (!IsDeadOrInvisible(pTarget) && (warheadTypeData->AffectShooter || pTarget != pAttacker))
				{
					// 可影响
					HouseClass* pTargetSourceHouse = GetSourceHouse(pTarget);
					if (CanAffectHouse(pAttackingHouse, pTargetSourceHouse, warheadTypeData->AffectsOwner, warheadTypeData->AffectsAllies, warheadTypeData->AffectsEnemies))
					{
						// 赋予AE
						AttachEffect* aeManager = nullptr;
						if (TryGetAEManager<BulletExt>(pTarget, aeManager))
						{
							aeManager->Attach(aeTypeData->AttachEffectTypes, aeTypeData->AttachEffectChances, false, pAttacker, pAttackingHouse, location);
						}
					}
				}
				return false;
				}, location, pWH->CellSpread, 0, fullAirspace);
		}
	}
}

void FindAndDamageStandOrVUnit(CoordStruct location, int damage,
	WarheadTypeClass* pWH, ObjectClass* pAttacker, HouseClass* pAttackingHouse, ObjectClass* exclude)
{
	double distance = pWH->CellSpread * Unsorted::LeptonsPerCell;
	std::map<TechnoClass*, DamageGroup> targets;
	// 检索爆炸范围内的替身
	for (auto standExt : TechnoExt::StandArray)
	{
		TechnoClass* pTarget = standExt.first;
		StandData data = standExt.second;
		DamageGroup damageGroup{};
		if (!data.Immune && pTarget != exclude
			&& CheckAndMarkTarget(pTarget, distance, location, damage, pAttacker, pWH, pAttackingHouse, damageGroup))
		{
			targets[pTarget] = damageGroup;
		}
	}
	// 检索爆炸范围内的虚单位
	for (auto pTarget : TechnoExt::VirtualUnitArray)
	{
		auto it = targets.find(pTarget);
		DamageGroup damageGroup{};
		if (it == targets.end() && pTarget != exclude
			&& CheckAndMarkTarget(pTarget, distance, location, damage, pAttacker, pWH, pAttackingHouse, damageGroup))
		{
			targets[pTarget] = damageGroup;
		}
	}
	// 炸了它
	for (auto t : targets)
	{
		DamageGroup damageGroup = t.second;
		t.first->ReceiveDamage(&damage, (int)damageGroup.Distance, pWH, pAttacker, false, false, pAttackingHouse);
	}

}

bool CheckAndMarkTarget(TechnoClass* pTarget, double distance, CoordStruct location, int damage, ObjectClass* pAttacker,
	WarheadTypeClass* pWH, HouseClass* pAttackingHouse, DamageGroup& damageGroup)
{
	if (pTarget && pTarget->GetType() && !IsImmune(pTarget))
	{
		// 检查距离
		CoordStruct targetPos = pTarget->GetCoords();
		double dist = targetPos.DistanceFrom(location);
		if (pTarget->WhatAmI() == AbstractType::Aircraft)
		{
			dist *= 0.5;
		}
		if (!isinf(dist) && dist <= distance)
		{
			int realDamage = 0;
			// 找到一个在范围内的目标，检查弹头是否可以影响该目标
			if (CanAffectMe(pTarget, pAttackingHouse, pWH)
				&& CanDamageMe(pTarget, damage, (int)dist, pWH, realDamage))
			{
				damageGroup.pTarget = pTarget;
				damageGroup.Distance = dist;
				return true;
			}
		}
	}
	return false;
}

