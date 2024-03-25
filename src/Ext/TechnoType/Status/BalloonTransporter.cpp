#include "../TechnoStatus.h"


void TechnoStatus::OnUpdate_BalloonTransporter()
{
	TechnoTypeClass* pType = pTechno->GetTechnoType();
	if (pType && pType->BalloonHover && pType->Passengers > 0 && pTechno->CurrentMission != Mission::Unload)
	{
		if (pTechno->Passengers.NumPassengers >= pType->Passengers)
		{
			BalloonFall = false;
			// 油满出发
			FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
			if (pFoot && !pFoot->Locomotor->Is_Moving() && !pFoot->Locomotor->Is_Moving_Now())
			{
				if (CellClass* pCell = MapClass::Instance->TryGetCellAt(pTechno->GetCoords()))
				{
					pFoot->SetDestination(pCell, true);
					pFoot->QueueMission(Mission::Move, true);
				}
			}
		}
	}
}

