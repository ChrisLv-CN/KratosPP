#include "../TechnoStatus.h"

#include <Ext/Helper/Status.h>
#include <Ext/Helper/Scripts.h>

PassengersData* TechnoStatus::GetPassengersData()
{
	if (!_passengersData)
	{
		_passengersData = INI::GetConfig<PassengersData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _passengersData;
}

/**
 *@brief 乘客只有在OpenTopped的载具内才可以执行Update事件
 *
 */
void TechnoStatus::OnUpdate_Passenger()
{
	if (!IsBuilding())
	{
		// 获取运输载具上设置的乘客行为
		if (TechnoClass* pTransporter = pTechno->Transporter)
		{
			TechnoStatus* status = nullptr;
			if (TryGetStatus<TechnoExt>(pTransporter, status))
			{
				PassengersData* data = status->GetPassengersData();
				if (!data->PassiveAcquire)
				{
					Mission transporterMission = pTransporter->GetCurrentMission();
					if (transporterMission != Mission::Attack)
					{
						pTechno->QueueMission(Mission::Sleep, true);
					}
				}
				if (data->ForceFire)
				{
					pTechno->SetTarget(pTransporter->Target);
				}
			}
		}
	}
}

bool TechnoStatus::CanFire_Passenger(AbstractClass* pTarget, WeaponTypeClass* pWeapon)
{
	bool ceaseFire = false;
	if (!IsBuilding())
	{
		// 获取运输载具上设置的乘客行为
		if (TechnoClass* pTransporter = pTechno->Transporter)
		{
			if (pTransporter->GetTechnoType()->OpenTopped)
			{
				TechnoStatus* status = nullptr;
				if (TryGetStatus<TechnoExt>(pTransporter, status))
				{
					PassengersData* data = status->GetPassengersData();
					switch (pTransporter->GetCurrentMission())
					{
					case Mission::Attack:
						ceaseFire = !data->SameFire;
						break;
					case Mission::Move:
					case Mission::AttackMove:
						ceaseFire = !data->MobileFire;
						break;
					}
				}
			}
		}
	}
	return ceaseFire;
}


