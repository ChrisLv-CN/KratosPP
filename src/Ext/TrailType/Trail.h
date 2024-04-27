#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INI.h>

#include <Ext/Helper/Status.h>

#include "TrailData.h"
#include "TrailTypeData.h"

struct Trail : public TrailData
{
public:
	Trail();
	Trail(TrailTypeData type);

	void SetDrivingState(DrivingState state);
	void SetLastLocation(CoordStruct location);
	void UpdateLastLocation(CoordStruct location);
	void ClearLastLocation();

	TrailMode GetMode();

	bool CanDraw();
	bool CheckVertical(CoordStruct sourcePos, CoordStruct targetPos);
	bool CheckInAir(bool isInAir);
	bool IsOnLand(CoordStruct sourcePos);

	void DrawTrail(CoordStruct currentPos, HouseClass* pHouse, TechnoClass* pCreater, bool isBullet = false);
	void RealDrawTrail(CoordStruct currentPos, CoordStruct behindPos, HouseClass* pHouse, TechnoClass* pCreater);

	void DrawAnimTrail(CoordStruct currentPos, CoordStruct behindPos, HouseClass* pHouse, TechnoClass* pCreater);
private:
	TrailTypeData type{};

	CoordStruct lastLocation = CoordStruct::Empty;
	bool canDraw = true;
	int initialDelay = 0;
	CDTimerClass delayTimer{};

	bool forceDraw = false;
	DrivingState drivingState = DrivingState::Moving;

	int laserColorIndex = 0;
};


// ----------------
// Helper
// ----------------

static bool TryGetImageTrails(const char* imageSection, std::vector<Trail>& trails)
{
	// 获取尾巴
	INIBufferReader* reader = INI::GetSection(INI::Art, imageSection);
	int i = -1;
	do
	{
		std::string title = "Trail";
		title += (i >= 0 ? std::to_string(i) : "");
		title += ".";
		std::string typeName = reader->Get(title + "Type", std::string{ "" });
		if (IsNotNone(typeName) && INI::HasSection(INI::Art, typeName.c_str()))
		{
			// 读取Section
			TrailTypeData* typeData = INI::GetConfig<TrailTypeData>(INI::Art, typeName.c_str())->Data;
			Trail trail{ *typeData };
			trail.Read(reader, title);
			trails.push_back(trail);
		}
		i++;
	} while (i < 11);
	return !trails.empty();
}

static bool TryGetTrails(const char* section, std::vector<Trail>& trails)
{
	// 获取Image的Section
	std::string image = INI::GetSection(INI::Rules, section)->Get("Image", std::string{ section });
	return TryGetImageTrails(image.c_str(), trails);
}
