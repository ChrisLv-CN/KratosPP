#pragma once
#include <vector>

#include <GeneralDefinitions.h>
#include <ParticleSystemTypeClass.h>
#include <TechnoClass.h>
#include <WeaponTypeClass.h>

#pragma region LaserType
struct LaserType
{
public:
	ColorStruct InnerColor{ 204, 64, 6 };
	ColorStruct OuterColor{ 102, 32, 3 };
	ColorStruct OuterSpread{ 0,0,0 };
	int Duration = 15;
	int Thickness = 2;
	bool IsHouseColor = false;
	bool IsSupported = false;
	bool Fade = true;

	std::vector<ColorStruct> ColorList{};
	bool ColorListRandom = false;
};

void DrawLaser(LaserType laser, CoordStruct sourcePos, CoordStruct targetPos, ColorStruct houseColor = Colors::Empty);
void DrawLaser(CoordStruct sourcePos, CoordStruct targetPos, ColorStruct innerColor, ColorStruct outerColor = Colors::Empty, int thickness = 2, int duration = 15);

namespace LaserHelper
{
	void RedLine(CoordStruct sourcePos, CoordStruct targetPos, int thickness = 1, int duration = 1);
	void GreenLine(CoordStruct sourcePos, CoordStruct targetPos, int thickness = 1, int duration = 1);
	void BlueLine(CoordStruct sourcePos, CoordStruct targetPos, int thickness = 1, int duration = 1);

	void RedLineZ(CoordStruct sourcePos, int length, int thickness = 1, int duration = 1);
	void GreenLineZ(CoordStruct sourcePos, int length, int thickness = 1, int duration = 1);
	void BlueLineZ(CoordStruct sourcePos, int length, int thickness = 1, int duration = 1);

	void Cell(CoordStruct sourcePos, int length, ColorStruct lineColor, ColorStruct outerColor = Colors::Empty, int thickness = 1, int duration = 1);
	void Crosshair(CoordStruct sourcePos, int length, ColorStruct lineColor, ColorStruct outerColor = Colors::Empty, int thickness = 1, int duration = 1);

	void RedCrosshair(CoordStruct sourcePos, int length = 128, int thickness = 1, int duration = 1);
	void GreenCrosshair(CoordStruct sourcePos, int length = 128, int thickness = 1, int duration = 1);
	void BlueCrosshair(CoordStruct sourcePos, int length = 128, int thickness = 1, int duration = 1);

	void RedCell(CoordStruct sourcePos, int length = 128, int thickness = 1, int duration = 1, bool crosshair = false);
	void GreenCell(CoordStruct sourcePos, int length = 128, int thickness = 1, int duration = 1, bool crosshair = false);
	void BlueCell(CoordStruct sourcePos, int length = 128, int thickness = 1, int duration = 1, bool crosshair = false);

	void MapCell(CoordStruct sourcePos, ColorStruct lineColor, ColorStruct outerColor = Colors::Empty, int thickness = 1, int duration = 1, bool crosshair = false);

	void RedMapCell(CoordStruct sourcePos, int thickness = 1, int duration = 1, bool crosshair = false);
	void GreenMapCell(CoordStruct sourcePos, int thickness = 1, int duration = 1, bool crosshair = false);
	void BlueMapCell(CoordStruct sourcePos, int thickness = 1, int duration = 1, bool crosshair = false);
}
#pragma endregion

#pragma region BeamType
class BeamType
{
public:
	BeamType(RadBeamType type)
	{
		SetBeamType(type);
	}

	void SetBeamType(RadBeamType type)
	{
		Type = type;
		switch (type)
		{
		case RadBeamType::Temporal:
			Color = { 128, 200, 255 };
			break;
		default:
			Color = { 0, 255, 0 };
			break;
		}
	}

	RadBeamType Type = RadBeamType::RadBeam;
	ColorStruct Color{ 0, 255, 0 }; // 绿色健康
	int Period = 15; // 周期
	double Amplitude = 40.0; // 振幅
};

void DrawBeam(CoordStruct sourcePos, CoordStruct targetPos, BeamType type, ColorStruct customColor = Colors::Empty);

void DrawRadBeam(CoordStruct sourcePos, CoordStruct targetPos, ColorStruct customColor = Colors::Empty, int period = 15, double amplitude = 40.0);
void DrawTemporalBeam(CoordStruct sourcePos, CoordStruct targetPos, ColorStruct customColor = Colors::Empty, int period = 15, double amplitude = 40.0);
#pragma endregion

#pragma region BoltType
class BoltType
{
public:
	BoltType(bool alternate)
	{
		IsAlternateColor = alternate;
	}

	bool IsAlternateColor = false;
	// custom color and line
	int ArcCount = 8;
	ColorStruct Color1 = Colors::Empty;
	ColorStruct Color2 = Colors::Empty;
	ColorStruct Color3 = Colors::Empty;
	bool Disable1 = false;
	bool Disable2 = false;
	bool Disable3 = false;
	bool DisableParticle = false;
};

void DrawBolt(CoordStruct sourcePos, CoordStruct targetPos, BoltType type);
void DrawBolt(CoordStruct sourcePos, CoordStruct targetPos, bool alternate = false);

void DrawBolt(TechnoClass* pShooter, AbstractClass* pTarget, WeaponTypeClass* pWeapon, CoordStruct sourcePos, CoordStruct flh, bool isOnTurret);
#pragma endregion

#pragma region ParticleSystem
void DrawParticle(ParticleSystemTypeClass* psType, CoordStruct &sourcePos, CoordStruct &targetPos, ObjectClass* pOwner = nullptr, AbstractClass* pTarget = nullptr, HouseClass* pHouse = nullptr);

void DrawParticle(const char* systemName, CoordStruct &sourcePos, CoordStruct &targetPos);
#pragma endregion
