#include "CrateBuffEffect.h"

#include <Ext/TechnoType/TechnoStatus.h>

void CrateBuffEffect::UpdateStatus()
{
	if (!AE->OwnerIsDead())
	{
		if (pTechno)
		{
			if (TechnoStatus* status = _gameObject->GetComponent<TechnoStatus>())
			{
				status->RecalculateStatus();
			}
		}
	}
}

void CrateBuffEffect::OnStart()
{
	UpdateStatus();
}

void CrateBuffEffect::End(CoordStruct location)
{
	UpdateStatus();
}

void CrateBuffEffect::OnPause()
{
	UpdateStatus();
}

