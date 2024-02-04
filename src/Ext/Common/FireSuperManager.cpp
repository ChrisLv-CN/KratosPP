#include "FireSuperManager.h"

void FireSuperManager::Order(HouseClass* pHouse, CoordStruct location, FireSuperEntity data)
{
	CellStruct targetPos = CellClass::Coord2Cell(location);
	HouseClass* house = pHouse == nullptr ? HouseClass::FindSpecial() : pHouse;
	FireSuper fireSuper{ house, targetPos, data };
	_superWeapons.emplace_back(fireSuper);
}

void FireSuperManager::Launch(HouseClass* pHouse, CoordStruct location, FireSuperEntity data)
{
	CellStruct targetPos = CellClass::Coord2Cell(location);
	RealLaunch(pHouse, targetPos, data);
}

void FireSuperManager::Clear(EventSystem* sender, Event e, void* args)
{
	_superWeapons.clear();
}

void FireSuperManager::Update(EventSystem* sender, Event e, void* args)
{
	if (args == nullptr) // update begin
	{
		int size = _superWeapons.size();
		for (int i = 0; i < size; i++)
		{
			auto it = _superWeapons.begin();
			FireSuper super = *it;
			_superWeapons.erase(it);
			// 发射武器
			if (super.CanLaunch())
			{
				RealLaunch(super.GetHouse(), super.TargetPos, super.Data);
				super.Cooldown();
			}
			if (!super.IsDone())
			{
				_superWeapons.emplace_back(super);
			}
		}
	}
}

void FireSuperManager::RealLaunch(HouseClass* pHouse, CellStruct targetPos, FireSuperEntity data)
{
	if (pHouse == nullptr || pHouse->Defeated)
	{
		// find civilian
		pHouse = HouseClass::FindCivilianSide();
		if (pHouse == nullptr)
		{
			return;
		}
	}
	// 发射
	int superCount = data.Supers.size();
	for (int index = 0; index < superCount; index++)
	{
		// 检查概率
		if (Bingo(data.Chances, index))
		{
			std::string superId = data.Supers[index];
			SuperWeaponTypeClass* pType = SuperWeaponTypeClass::Find(superId.c_str());
			if (pType)
			{
				SuperClass* pSuper = FindSuperWeapon(pHouse, pType);
				if (pSuper->IsCharged || !data.RealLaunch)
				{
					pSuper->IsCharged = true;
					pSuper->Launch(targetPos, pHouse->IsControlledByHuman());
					pSuper->IsCharged = false;
					pSuper->Reset();
				}
			}
		}
	}
}

SuperClass* FireSuperManager::FindSuperWeapon(HouseClass* pHouse, SuperWeaponTypeClass* pType)
{
	for (int i = 0; i < pHouse->Supers.Count; ++i) {
		auto pItem = pHouse->Supers.Items[i];
		if (pHouse->Supers.Items[i]->Type == pType) {
			return pItem;
		}
	}
	return nullptr;
}
