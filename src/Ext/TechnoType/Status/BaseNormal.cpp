#include "../TechnoStatus.h"

#include <Ext/Common/CommonStatus.h>

BaseNormalData* TechnoStatus::GetBaseNormalData()
{
	if (!_baseNormalData)
	{
		_baseNormalData = INI::GetConfig<BaseNormalData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _baseNormalData;
}

void TechnoStatus::ResetBaseNormal()
{
	_baseNormalData = nullptr;
	OnRemove_BaseNormarl();
	OnPut_BaseNormarl(nullptr, DirType::North);
}

void TechnoStatus::OnPut_BaseNormarl(CoordStruct* pLocation, DirType dir)
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

void TechnoStatus::OnUpdate_BaseNormal()
{
	// 读档后重新将自己加入清单中
	if (!_baseNormalData)
	{
		ResetBaseNormal();
	}
}

void TechnoStatus::OnRemove_BaseNormarl()
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

void TechnoStatus::OnReceiveDamageDestroy_BaseNormarl()
{
	OnRemove_BaseNormarl();
}
