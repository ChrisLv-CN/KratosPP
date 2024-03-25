#include "AircraftPut.h"

#include <Ext/Helper/Finder.h>

AircraftPutData* AircraftPut::GetAircraftPutData()
{
	if (!_data)
	{
		_data = INI::GetConfig<AircraftPutData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _data;
}

int AircraftPut::CountAircraft()
{
	int count = 0;
	auto array = AircraftClass::Array.get();
	auto padList = GetAircraftPutData()->PadAircraft;
	for (int i = array->Count - 1; i >= 0; i--)
	{
		AircraftClass* pTarget = array->GetItem(i);
		if (pTarget->Owner == pTechno->Owner)
		{
			auto it = std::find(padList.begin(), padList.end(), pTarget->Type->ID);
			if (it != padList.end())
			{
				count++;
			}
		}
	}
	return count;
}

void AircraftPut::Awake()
{
	if (!IsAircraft() || !GetAircraftPutData()->Enable
		|| !GetAircraftPutData()->NeedPad(pTechno->GetTechnoType()->ID))
	{
		Disable();
	}
}

void AircraftPut::OnPut(CoordStruct* pCoord, DirType dirType)
{
	// 检查停机坪数量
	if (dynamic_cast<AircraftClass*>(pTechno)->Type->AirportBound)
	{
		HouseClass* pHouse = pTechno->Owner;
		if (pHouse->AirportDocks <= 0 || pHouse->AirportDocks < CountAircraft())
		{
			// 停机坪满了，不移动飞机的位置
			Disable();
			return;
		}
	}
	// 调整飞机的出生点位
	AircraftPutData* data = GetAircraftPutData();
	if (!data->ForcePutOffset)
	{
		// check Building has Helipad or not
		if (CellClass* pCell = MapClass::Instance->TryGetCellAt(*pCoord))
		{
			if (BuildingClass* pBuilding = pCell->GetBuilding())
			{
				if (pBuilding->Type->Helipad)
				{
					// 脚下的建筑有停机坪，不移动飞机的位置
					Disable();
					return;
				}
			}
		}
	}
	// 修改完put的坐标，又会被机场修改回去，所以必须要在update里进行位置调整
	CoordStruct offset = data->NoHelipadPutOffset;
	offset *= Unsorted::LeptonsPerCell;
	*pCoord += offset;
	_aircraftPutOffset = true;
}


void AircraftPut::OnUpdate()
{
	if (_aircraftPutOffset)
	{
		// 动一下
		_aircraftPutOffset = false;
		CoordStruct location = pTechno->GetCoords();
		CoordStruct offset = GetAircraftPutData()->NoHelipadPutOffset;
		offset *= Unsorted::LeptonsPerCell;
		CoordStruct pos = location + offset;
		pTechno->SetLocation(pos);
		if (CellClass* pCell = MapClass::Instance->TryGetCellAt(pos))
		{
			pTechno->SetDestination(pCell, true);
		}
		pTechno->QueueMission(Mission::Enter, false);
	}
	// 完成使命
	Disable();
}

