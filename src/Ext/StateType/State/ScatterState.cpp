#include "ScatterState.h"

void ScatterState::OnUpdate()
{
	if (!IsBuilding())
	{
		if (IfReset())
		{
			_forceMoving = false;
			_panic = false;
		}
		_panic = Data.Panic;
		if (_panic && IsInfantry())
		{
			InfantryClass* pInfantry = dynamic_cast<InfantryClass*>(pTechno);
			if (pInfantry->PanicDurationLeft <= 200)
			{
				pInfantry->PanicDurationLeft = 300;
			}
		}
		// 向目标移动，再散开
		if (!_forceMoving && !Data.MoveToFLH.IsEmpty())
		{
			_forceMoving = true;
			CoordStruct moveTo = GetFLHAbsoluteCoords(pTechno, Data.MoveToFLH, false);
			if (CellClass* pTargetCell = MapClass::Instance->GetCellAt(moveTo))
			{
				pTechno->SetDestination(pTargetCell, true);
				pTechno->QueueMission(Mission::Move, true);
			}
		}
		else if (CellClass* pCell = MapClass::Instance->GetCellAt(pTechno->GetCoords()))
		{
			pTechno->Scatter(pCell->GetCoordsWithBridge(), true, false);
		}
	}
	else
	{
		_forceMoving = false;
		_panic = false;
	}
}
