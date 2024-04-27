#include "Trail.h"

#include <Ext/Helper/DrawEx.h>
#include <Ext/Helper/FLH.h>
#include <Ext/Helper/MathEx.h>

Trail::Trail() {}

Trail::Trail(TrailTypeData type)
{
	this->type = type;
	if (type.InitialDelay > 0)
	{
		this->canDraw = false;
		this->initialDelay = type.InitialDelay;
	}
}

void Trail::SetDrivingState(DrivingState state)
{
	this->drivingState = state;
	if (state == DrivingState::Stop)
	{
		this->forceDraw = true;
	}
}

void Trail::SetLastLocation(CoordStruct location)
{
	this->lastLocation = location;
}

void Trail::UpdateLastLocation(CoordStruct location)
{
	double dist = location.DistanceFrom(lastLocation);
	if (!isnan(dist) && dist > type.Distance || forceDraw)
	{
		this->lastLocation = location;
	}
}

void Trail::ClearLastLocation()
{
	this->lastLocation = CoordStruct::Empty;
}

TrailMode Trail::GetMode()
{
	return type.Mode;
}

bool Trail::CanDraw()
{
	if (!canDraw)
	{
		if (initialDelay > 0)
		{
			delayTimer.Start(initialDelay);
			initialDelay = 0;
		}
		canDraw = delayTimer.Expired();
	}
	return canDraw;
}

bool Trail::CheckVertical(CoordStruct sourcePos, CoordStruct targetPos)
{
	return (type.IgnoreVertical ? (abs(sourcePos.X - targetPos.X) > 32 || abs(sourcePos.Y - targetPos.Y) > 32) : true);
}

bool Trail::CheckInAir(bool isInAir)
{
	switch (DrawLevel)
	{
	case TrailDrawing::LAND:
		return !isInAir;
	case TrailDrawing::AIR:
		return isInAir;
	default:
		return true;
	}
}

bool Trail::IsOnLand(CoordStruct sourcePos)
{
	bool canDraw = true;
	if (CellLimit)
	{
		canDraw = false;
		if (CellClass* pCell = MapClass::Instance->TryGetCellAt(sourcePos))
		{
			if (!OnLandTypes.empty())
			{
				// 特殊过滤桥面
				LandType landType = pCell->LandType;
				if (landType == LandType::Water && pCell->Flags & CellFlags::Bridge)
				{
					// 将水面上的桥强制判定为路面
					landType = LandType::Road;
				}
				canDraw = std::find(OnLandTypes.begin(), OnLandTypes.end(), landType) != OnLandTypes.end();
			}
			// 检查地块
			if (!canDraw && !OnTileTypes.empty())
			{
				canDraw = std::find(OnTileTypes.begin(), OnTileTypes.end(), pCell->GetTileType()) != OnTileTypes.end();
			}
		}
	}
	return canDraw;
}

void Trail::DrawTrail(CoordStruct currentPos, HouseClass* pHouse, TechnoClass* pCreater, bool isBullet)
{
	if (!currentPos.IsEmpty())
	{
		if (lastLocation.IsEmpty())
		{
			lastLocation = currentPos;
		}
		else
		{
			CoordStruct behindPos = lastLocation;
			double dist = currentPos.DistanceFrom(behindPos);
			if (!isnan(dist) && dist > type.Distance || forceDraw)
			{
				if ((CanDraw() && CheckVertical(currentPos, behindPos) && CheckInAir(isBullet || pCreater->IsInAir())) || forceDraw)
				{
					forceDraw = false;
					if (IsOnLand(currentPos))
					{
						RealDrawTrail(currentPos, behindPos, pHouse, pCreater);
					}
					drivingState = DrivingState::Moving;
				}
				lastLocation = currentPos;
			}
		}
	}
}

void Trail::RealDrawTrail(CoordStruct currentPos, CoordStruct behindPos, HouseClass* pHouse, TechnoClass* pCreater)
{
	switch (type.Mode)
	{
	case TrailMode::LASER:
	{
		ColorStruct houseColor = Colors::Empty;
		if (pHouse)
		{
			houseColor = pHouse->LaserColor;
		}
		// 修改渲染的颜色
		int count = type.LaserType.ColorList.size();
		if (count > 0)
		{
			// 随机
			laserColorIndex = type.LaserType.ColorListRandom ? Random::RandomRanged(0, count - 1) : laserColorIndex;
			ColorStruct color = type.LaserType.ColorList[laserColorIndex];
			if (++laserColorIndex >= count)
			{
				laserColorIndex = 0;
			}
			type.LaserType.InnerColor = color;
			DrawLaser(type.LaserType, currentPos, behindPos);
		}
		else
		{
			DrawLaser(type.LaserType, currentPos, behindPos, type.LaserType.IsHouseColor ? houseColor : Colors::Empty);
		}
		break;
	}
	case TrailMode::ELECTIRIC:
		DrawBolt(currentPos, behindPos, type.BoltType);
		break;
	case TrailMode::BEAM:
		DrawBeam(currentPos, behindPos, type.BeamType);
		break;
	case TrailMode::PARTICLE:
		DrawParticle(type.ParticleSystem.c_str(), currentPos, behindPos);
		break;
	case TrailMode::ANIM:
		DrawAnimTrail(currentPos, behindPos, pHouse, pCreater);
		break;
	}
}

void Trail::DrawAnimTrail(CoordStruct currentPos, CoordStruct behindPos, HouseClass* pHouse, TechnoClass* pCreater)
{
	std::vector<std::string>* animTypes = &type.WhileDrivingAnim;
	switch (drivingState)
	{
	case DrivingState::Start:
		animTypes = &type.StartDrivingAnim;
		break;
	case DrivingState::Stop:
		animTypes = &type.StopDrivingAnim;
		break;
	}
	std::string animType;
	// 随机或者按方向获取
	if (animTypes->size() > 0)
	{
		int facing = animTypes->size();
		int index = 0;
		if (facing > 1)
		{
			if (facing % 8 == 0)
			{
				CoordStruct tempCurrentPos = currentPos;
				tempCurrentPos.Z = 0;
				CoordStruct tempBehindPos = behindPos;
				tempBehindPos.Z = 0;
				DirStruct dir = Point2Dir(tempBehindPos, tempCurrentPos);
				index = Dir2FrameIndex(dir, facing);
			}
			else
			{
				index = Random::RandomRanged(0, facing - 1);
			}
		}
		animType = animTypes->at(index);
	}
	if (IsNotNone(animType))
	{
		AnimTypeClass* pAnimType = AnimTypeClass::Find(animType.c_str());
		if (pAnimType)
		{
			AnimClass* pAnim = GameCreate<AnimClass>(pAnimType, currentPos);
			pAnim->Owner = pHouse;
			SetAnimCreater(pAnim, pCreater);
		}
	}
}
