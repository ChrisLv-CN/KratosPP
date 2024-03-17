#include "AircraftDive.h"

#include <FlyLocomotionClass.h>

#include <Ext/Helper/Finder.h>

AircraftDiveData* AircraftDive::GetAircraftDiveData()
{
	if (!_data)
	{
		_data = INI::GetConfig<AircraftDiveData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _data;
}

void AircraftDive::Setup()
{
	_data = nullptr;
	if (!IsAircraft() || !IsFly() || !GetAircraftDiveData()->Enable)
	{
		Disable();
	}
}

void AircraftDive::Awake()
{
	Setup();
}

void AircraftDive::ExtChanged()
{
	Setup();
}


void AircraftDive::OnUpdate()
{
	if (!IsDeadOrInvisible(pTechno) && !dynamic_cast<AircraftClass*>(pTechno)->Type->Carryall)
	{
		AircraftDiveData* data = GetAircraftDiveData();
		FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
		FlyLocomotionClass* pFly = dynamic_cast<FlyLocomotionClass*>(pFoot->Locomotor.get());
		if (!pTechno->IsInAir() || pFly->IsTakingOff || pFly->IsLanding)
		{
			// 归零
			DiveStatus = AircraftDiveStatus::NONE;
			_activeDive = false;
			// 解锁飞行姿态
			Attitude()->UnLock();
			return;
		}
		// 带蛋起飞，并且高度超过设定值时，开启俯冲
		if (pFly->IsElevating && pTechno->GetHeight() >= data->FlightLevel)
		{
			_activeDive = true;
		}
		AbstractClass* pTarget = pTechno->Target;
		switch (DiveStatus)
		{
		case AircraftDiveStatus::DIVEING:
			if (!pTarget)
			{
				DiveStatus = AircraftDiveStatus::PULLUP;
			}
			else
			{
				if (data->HeadToTarget)
				{
					// 当飞行高度接近目标高度的时候，解除飞行姿态控制
					int height = pTechno->GetHeight() + Unsorted::LevelHeight;
					if (height <= data->FlightLevel)
					{
						Attitude()->UnLock();
					}
					else
					{
						// 持续保持头对准目标
						Attitude()->UpdateHeadToCoord(pTarget->GetCoords(), true);
					}
				}
			}
			break;
		case AircraftDiveStatus::PULLUP:
			// 恢复飞行高度
			DiveStatus = AircraftDiveStatus::NONE;
			break;
		default:
			// 开始冲
			// 解锁飞行姿态
			Attitude()->UnLock();
			if (pTarget && _activeDive)
			{
				// 检查距离目标的距离是否足够近以触发俯冲姿态
				CoordStruct location = pTechno->GetCoords();
				CoordStruct targetPos = pTarget->GetCoords();
				int distance = (int)(data->Distance * Unsorted::LeptonsPerCell);
				if (distance == 0)
				{
					int weaponIndex = pTechno->SelectWeapon(pTarget);
					distance = pTechno->GetWeapon(weaponIndex)->WeaponType->Range * 2; // 预留两倍武器射程做俯冲缓冲
				}
				double dist = 0;
				if ((dist = location.DistanceFrom(targetPos)) <= distance)
				{
					// 进入俯冲状态
					DiveStatus = AircraftDiveStatus::DIVEING;
					// 调整飞行高度
					pFly->FlightLevel = data->FlightLevel;
					if (data->HeadToTarget)
					{
						// 头对准目标
						Attitude()->UpdateHeadToCoord(pTarget->GetCoords(), true);
					}
				}
			}
			break;
		}
	}
}

void AircraftDive::OnFire(AbstractClass* pTarget, int weaponIdx)
{
	if (GetAircraftDiveData()->PullUpAfterFire && DiveStatus == AircraftDiveStatus::DIVEING)
	{
		DiveStatus = AircraftDiveStatus::PULLUP;
	}
}

