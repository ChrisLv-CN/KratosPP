#include "../TechnoStatus.h"

void TechnoStatus::SetAirstrike(AirstrikeClass* airstrike)
{
	if (_airstrikes.empty())
	{
		EventSystems::General.AddHandler(Events::DetachAll, this, &TechnoStatus::OnAirstrikeDetach);
	}
	if (std::find(_airstrikes.begin(), _airstrikes.end(), airstrike) == _airstrikes.end())
	{
		_airstrikes.emplace_back(airstrike);
	}
	pTechno->NeedsRedraw = true;
}

void TechnoStatus::CancelAirstrike(AirstrikeClass* airstrike)
{
	for (auto it = _airstrikes.begin(); it != _airstrikes.end();)
	{
		if (*it == airstrike)
		{
			it = _airstrikes.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}
	if (_airstrikes.empty())
	{
		pTechno->NeedsRedraw = true;
		EventSystems::General.RemoveHandler(Events::DetachAll, this, &TechnoStatus::OnAirstrikeDetach);
	}
}

bool TechnoStatus::AnyAirstrike()
{
	return !_airstrikes.empty();
}
