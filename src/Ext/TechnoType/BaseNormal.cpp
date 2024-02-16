#include "BaseNormal.h"

#include <Ext/Helper/Scripts.h>

#include <Ext/TechnoType/TechnoStatus.h>

BaseNormalData* BaseNormal::GetBaseNormalData()
{
	if (!_baseNormalData)
	{
		_baseNormalData = INI::GetConfig<BaseNormalData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _baseNormalData;
}

void BaseNormal::Setup()
{
	_baseNormalData = nullptr;
	OnRemove();
	if (!IsBuilding() && GetBaseNormalData()->Enable)
	{
		OnPut(nullptr, DirType::North);
	}
	else
	{
		Disable();
	}
}

bool BaseNormal::AmIStand()
{
	TechnoStatus* status = nullptr;
	if (TryGetStatus<TechnoExt>(pTechno, status))
	{
		return status->AmIStand();
	}
	return false;
}

void BaseNormal::Awake()
{
	Setup();
}

void BaseNormal::Destroy()
{
	OnRemove();
}

void BaseNormal::ExtChanged()
{
	Setup();
}

void BaseNormal::OnPut(CoordStruct* pCoord, DirType dir)
{
	if (!IsBuilding() && GetBaseNormalData()->BaseNormal)
	{
		if (AmIStand())
		{
			TechnoExt::BaseStandArray[pTechno] = GetBaseNormalData()->EligibileForAllyBuilding;
		}
		else
		{
			TechnoExt::BaseUnitArray[pTechno] = GetBaseNormalData()->EligibileForAllyBuilding;
		}
	}
}

void BaseNormal::OnUpdate()
{
	// 读档后重新将自己加入清单中
	if (!_baseNormalData)
	{
		Setup();
	}
}

void BaseNormal::OnRemove()
{
	auto it = TechnoExt::BaseStandArray.find(pTechno);
	if (it != TechnoExt::BaseStandArray.end())
	{
		TechnoExt::BaseStandArray.erase(it);
	}
	auto it2 = TechnoExt::BaseUnitArray.find(pTechno);
	if (it2 != TechnoExt::BaseUnitArray.end())
	{
		TechnoExt::BaseUnitArray.erase(it);
	}
}

void BaseNormal::OnReceiveDamageDestroy()
{
	OnRemove();
}

