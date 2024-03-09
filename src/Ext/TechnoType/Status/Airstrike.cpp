#include "../TechnoStatus.h"

void TechnoStatus::SetAirstrike(AirstrikeClass* airstrike)
{
	if (!Airstrike)
	{
		EventSystems::General.AddHandler(Events::DetachAll, this, &TechnoStatus::OnAirstrikeDetach);
	}
	Airstrike = airstrike;
	pTechno->NeedsRedraw = true;
}

void TechnoStatus::CancelAirstrike()
{
	Airstrike = nullptr;
	pTechno->NeedsRedraw = true;
	EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnAirstrikeDetach);
}
