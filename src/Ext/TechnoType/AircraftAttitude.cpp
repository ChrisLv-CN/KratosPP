#include "AircraftAttitude.h"

#include <FlyLocomotionClass.h>

#include <Ext/Helper/DrawEx.h>
#include <Ext/Helper/Finder.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>

#include <Ext/Common/PrintTextManager.h>

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
			if (dockData->Enable)
			{
				auto it = dockData->Direction.find(index);
				if (it != dockData->Direction.end())
				{
					poseDir = it->second;
					return true;
				}
			}
		}
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
		FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
		FlyLocomotionClass* pFly = dynamic_cast<FlyLocomotionClass*>(pFoot->Locomotor.get());

		if (pFly->IsTakingOff || pFly->IsLanding || !pFly->HasMoveOrder)
		{
			PitchAngle = 0;
			return;
		}
		int height = pTechno->GetHeight();
		CellClass* pCell = MapClass::Instance->TryGetCellAt(_location);
		if (pCell && pCell->ContainsBridge())
		{
			CoordStruct cellPos = pCell->GetCoords();
			height -= cellPos.Z;
		}
		// 飞机在全力爬升或者下降，不修正抖动
		int z = height - pFly->FlightLevel;
		if (abs(z) <= Unsorted::LevelHeight)
		{
			// 检查是在倾斜地面上平飞还是爬坡，检查前后两个地块的高度没有明显的偏差
			CellClass* pNextCell = MapClass::Instance->TryGetCellAt(headTo);
			if (pCell && pNextCell && pNextCell != pCell)
			{
				CoordStruct pos1 = pCell->GetCoordsWithBridge();
				CoordStruct pos2 = pNextCell->GetCoordsWithBridge();
				int zCell = abs(pos1.Z - pos2.Z);
				if (zCell == 0 || zCell <= Unsorted::LevelHeight || (pCell->Flags & CellFlags::Bridge || pNextCell->Flags & CellFlags::Bridge))
				{
					// 平飞
					_targetAngle = 0;
					return;
				}
			}
		}

		// 计算角度
		int deltaZ = _location.Z - headTo.Z; // 高度差
		int hh = abs(deltaZ);
		if (z == 0 && hh < 20) // 消除小幅度的抖动，每帧高度变化最大20
		{
			// 平飞
			_targetAngle = 0;
		}
		else
		{
			// 计算俯仰角度
			double dist = headTo.DistanceFrom(_location);
			if (isnan(dist) || isinf(dist))
			{
				// 无法计算距离，用飞行速度代替
				dist = pFly->CurrentSpeed;
			}
			double angle = Math::asin(hh / dist);
			if (deltaZ > 0 || z > 0)
			{
				// 俯冲
				_targetAngle = (float)angle;
			}
			else if (deltaZ < 0 || z < 0)
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
	_location = pTechno->GetCoords();
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
		// 检查是否开启了吊运功能
		if (dynamic_cast<AircraftClass*>(pTechno)->Type->Carryall)
		{
			PitchAngle = 0;
		}
		else
		{
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
		}
		// 关闭图像缓存
		GetTechnoStatus()->DisableVoxelCache = PitchAngle != 0;
		_location = pTechno->GetCoords();
		if (!GetAircraftAttitudeData()->Disable && !_lockAngle)
		{
			// 根据速度计算出飞行的下一个位置
			FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
			if (FlyLocomotionClass* pFly = dynamic_cast<FlyLocomotionClass*>(pFoot->Locomotor.get()))
			{
				CoordStruct destPos = CoordStruct::Empty;
				pFoot->Locomotor->Destination(&destPos);
				if (!destPos.IsEmpty())
				{
					DirStruct dir = pTechno->SecondaryFacing.Current();
					// 目的地和当前不在一个格子内，取飞机头部前方的格子
					if (CellClass::Coord2Cell(destPos) != CellClass::Coord2Cell(_location))
					{
						int f = 0;
						if (pFoot->Locomotor->Is_Moving())
						{
							f = Unsorted::LeptonsPerCell;
						}
						// 飞机使用的是炮塔角度
						destPos = GetFLHAbsoluteCoords(_location, CoordStruct{ f, 0,0 }, dir);
					}
					// 检查与前一格的高度差
					int state = 0;
					if (CellClass* pCell = MapClass::Instance->TryGetCellAt(destPos))
					{
						destPos.Z = pCell->GetCoordsWithBridge().Z + pFly->FlightLevel;
						if (destPos.Z > _location.Z)
						{
							// 爬升
							state = 1;
						}
						else if (destPos.Z < _location.Z)
						{
							// 俯冲
							state = -1;
						}
					}
					// 取飞机前方的坐标，高度设置为下一个坐标所处的格子的高度差+-20，游戏上升下降的速度是20
					CoordStruct nextPos = CoordStruct::Empty;
					int speed = pFly->Apparent_Speed();
					nextPos = GetFLHAbsoluteCoords(_location, CoordStruct{ speed, 0, 0 }, dir);
					switch (state)
					{
					case 1:
						nextPos.Z += 20;
						break;
					case -1:
						nextPos.Z -= 20;
						break;
					}
					UpdateHeadToCoord(nextPos);
				}
				else
				{
					PitchAngle = 0;
				}
			}
		}
	}
}

void AircraftAttitude::OnUpdateEnd()
{
	// 子机在降落和起飞时调整鸡头的朝向
	if (!IsDeadOrInvisible(pTechno) && !IsDeadOrInvisible(pTechno->SpawnOwner))
	{
		AircraftAttitudeData* data = GetAircraftAttitudeData();
		FootClass* pFoot = dynamic_cast<FootClass*>(pTechno);
		FlyLocomotionClass* pFly = dynamic_cast<FlyLocomotionClass*>(pFoot->Locomotor.get());

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


