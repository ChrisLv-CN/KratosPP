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

void CrateBuffEffect::End(CoordStruct location)
{
	UpdateStatus();
}

void CrateBuffEffect::OnPause()
{
	_updateFlag = false;
}

void CrateBuffEffect::OnRecover()
{
	_updateFlag = false;
}

void CrateBuffEffect::OnUpdate()
{
	if (!_updateFlag)
	{
		_updateFlag = true;
		UpdateStatus();
	}
}
