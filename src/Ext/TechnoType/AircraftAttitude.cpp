#include "AircraftAttitude.h"

#include <FlyLocomotionClass.h>

#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>

AircraftAttitudeData* AircraftAttitude::GetAircraftAttitudeData()
{
	if (!_data)
	{
		_data = INI::GetConfig<AircraftAttitudeData>(INI::Rules, pTechno->GetTechnoType()->ID)->Data;
	}
	return _data;
}

bool AircraftAttitude::TryGetAirportDir(int& poseDir)
{
	poseDir = RulesClass::Instance->PoseDir;
	AircraftClass* pAir = dynamic_cast<AircraftClass*>(pTechno);
	if (pAir->HasAnyLink()) // InRadioContact
	{
		TechnoClass* pAirport = pAir->GetNthLink();
		// 傻逼飞机是几号停机位
		int index = dynamic_cast<RadioClass*>(pAirport)->FindLinkIndex(pAir);
		if (index < 128)
		{
			const char* section = pAirport->GetTechnoType()->ID;
			std::string image = INI::GetSection(INI::Rules, section)->Get("Image", std::string{ section });
			AircraftDockingOffsetData* dockData = INI::GetConfig<AircraftDockingOffsetData>(INI::Art, image.c_str())->Data;
			std::vector<int> dirs = dockData->Direction;
			if (index < (int)dirs.size())
			{
				poseDir = dirs[index];
			}
		}
		return true;
	}
	return false;
}

void AircraftAttitude::UnLock()
{
	_smooth = true;
	_lockAngle = false;
}

void AircraftAttitude::UpdateHeadToCoord(CoordStruct headTo, bool lockAngle)
{
	if (lockAngle)
	{
		_smooth = false;
		_lockAngle = true;
	}
	if (IsDeadOrInvisibleOrCloaked(pTechno) || !pTechno->IsInAir())
	{
		PitchAngle = 0;
		return;
	}
	if (!headTo.IsEmpty())
	{
		// 检查是在倾斜地面上平飞还是爬坡，检查前后两个地块的高度没有明显的偏差
		AircraftAttitudeData* data = GetAircraftAttitudeData();
		FootClass* pFoot = static_cast<FootClass*>(pTechno);
		FlyLocomotionClass* pFly = static_cast<FlyLocomotionClass*>(pFoot->Locomotor.get());

		if (pFly->IsTakingOff || pFly->IsLanding || !pFly->HasMoveOrder)
		{
			PitchAngle = 0;
			return;
		}
		// 计算角度
		int z = pTechno->GetHeight() - pFly->FlightLevel; // 上升还是下降
		double deltaZ = _lastLocation.Z - headTo.Z; // 高度差
		double zz = abs(deltaZ);
		if (z == 0 || zz < 20) // 消除小幅度的抖动
		{
			// 平飞
			_targetAngle = 0;
		}
		else
		{
			// 计算俯仰角度
			double dist = _lastLocation.DistanceFrom(headTo);
			double angle = Math::asin(zz / dist);
			if (z > 0)
			{
				// 俯冲
				_targetAngle = (float)angle;
			}
			else
			{
				// 爬升
				_targetAngle = -(float)angle;
			}
		}
	}
}

void AircraftAttitude::Setup()
{
	_data = nullptr;
	if (!IsAircraft() || !IsFly() || !pTechno->IsVoxel())
	{
		Disable();
	}
	PitchAngle = 0.0;

	_targetAngle = 0.0;
	_smooth = true;
	_lockAngle = false;
	_lastLocation = pTechno->GetCoords();
}

void AircraftAttitude::Awake()
{
	Setup();
}

void AircraftAttitude::ExtChanged()
{
	Setup();
}

void AircraftAttitude::OnUpdate()
{
	// 调整飞机出厂时的朝向
	if (!_initFlag)
	{
		_initFlag = true;
		int dir = 0;
		if (TryGetAirportDir(dir))
		{
			DirStruct dirStruct = DirNormalized(dir, 8);
			pTechno->PrimaryFacing.SetCurrent(dirStruct);
			pTechno->SecondaryFacing.SetCurrent(dirStruct);
		}
	}
	if (!IsDeadOrInvisible(pTechno))
	{
		// WWSB 飞机在天上，Mission变成了Sleep
		if (pTechno->IsInAir() && pTechno->GetCurrentMission() == Mission::Sleep)
		{
			if (pTechno->Target)
			{
				pTechno->ForceMission(Mission::Attack);
			}
			else
			{
				pTechno->ForceMission(Mission::Enter);
			}
		}
		// 正事
		// 角度差比Step大
		if (_smooth && PitchAngle != _targetAngle && abs(_targetAngle - PitchAngle) > angelStep)
		{
			// 只调整一个step
			if (_targetAngle > PitchAngle)
			{
				PitchAngle += angelStep;
			}
			else
			{
				PitchAngle -= angelStep;
			}
		}
		else
		{
			PitchAngle = _targetAngle;
		}
		// 关闭图像缓存
		GetStaus()->DisableVoxelCache = PitchAngle != 0;
		CoordStruct location = pTechno->GetCoords();
		if (!GetAircraftAttitudeData()->Disable && !_lockAngle)
		{
			UpdateHeadToCoord(location);
		}
		_lastLocation = location;
	}
}

void AircraftAttitude::OnUpdateEnd()
{
	// 子机在降落和起飞时调整鸡头的朝向
	if (!IsDeadOrInvisible(pTechno) && !IsDeadOrInvisible(pTechno->SpawnOwner))
	{
		AircraftAttitudeData* data = GetAircraftAttitudeData();
		FootClass* pFoot = static_cast<FootClass*>(pTechno);
		FlyLocomotionClass* pFly = static_cast<FlyLocomotionClass*>(pFoot->Locomotor.get());

		TechnoClass* pSpawnOwner = pTechno->SpawnOwner;

		int dir = 0;
		if (pFly->IsLanding)
		{
			dir = data->SpawnLandDir;
			DirStruct dirStruct = GetRelativeDir(pSpawnOwner, dir, false);
			pTechno->PrimaryFacing.SetDesired(dirStruct);
			pTechno->SecondaryFacing.SetDesired(dirStruct);
		}
		else if (pFly->HasMoveOrder && !pTechno->IsInAir() && pSpawnOwner->GetTechnoType()->RadialFireSegments <= 1)
		{
			switch (pTechno->GetCurrentMission())
			{
			case Mission::Guard:
			case Mission::Area_Guard:
				dir = data->SpawnTakeoffDir;
				DirStruct dirStruct = GetRelativeDir(pSpawnOwner, dir, false);
				pTechno->PrimaryFacing.SetCurrent(dirStruct);
				pTechno->SecondaryFacing.SetCurrent(dirStruct);
				break;
			}
		}
	}
}

void AircraftAttitude::OnFire(AbstractClass* pTarget, int weaponIdx)
{

}

