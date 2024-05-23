#pragma once

#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Helper/DrawEx.h>


enum class TrailMode
{
	NONE = 0, LASER = 1, ELECTIRIC = 2, BEAM = 3, PARTICLE = 4, ANIM = 5
};

template <>
inline bool Parser<TrailMode>::TryParse(const char* pValue, TrailMode* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case 'L':
		*outValue = TrailMode::LASER;
		return true;
	case 'E':
		*outValue = TrailMode::ELECTIRIC;
		return true;
	case 'B':
		*outValue = TrailMode::BEAM;
		return true;
	case 'P':
		*outValue = TrailMode::PARTICLE;
		return true;
	case 'A':
		*outValue = TrailMode::ANIM;
		return true;
	}
	return false;
}

/**
 *@brief 尾巴类型设置，单独section
 *
 */
class TrailTypeData : public INIConfig
{
public:
	// 通用设置
	TrailMode Mode = TrailMode::LASER;
	int Distance = 64;
	bool IgnoreVertical = false;
	int InitialDelay = 0;

	// 不同类型的细节设置
	LaserType LaserType{}; // 激光尾巴
	BoltType BoltType{ false }; // 电弧尾巴
	BeamType BeamType{ RadBeamType::RadBeam }; // 辐射尾巴
	std::string ParticleSystem; // 粒子尾巴

	// 动画尾巴
	std::vector<std::string> StartDrivingAnim{};
	std::vector<std::string> WhileDrivingAnim{};
	std::vector<std::string> StopDrivingAnim{};

	virtual void Read(INIBufferReader* reader) override
	{
		Mode = reader->Get("Mode", Mode);
		Distance = reader->Get("Distance", Distance);
		IgnoreVertical = reader->Get("IgnoreVertical", IgnoreVertical);
		InitialDelay = reader->Get("InitialDelay", InitialDelay);

		// 激光尾巴
		LaserType.InnerColor = reader->Get("Laser.InnerColor", LaserType.InnerColor);
		LaserType.OuterColor = reader->Get("Laser.OuterColor", LaserType.OuterColor);
		LaserType.OuterSpread = reader->Get("Laser.OuterSpread", LaserType.OuterSpread);

		LaserType.IsHouseColor = reader->Get("Laser.IsHouseColor", LaserType.IsHouseColor);
		LaserType.IsSupported = reader->Get("Laser.IsSupported", LaserType.IsSupported);
		LaserType.Fade = reader->Get("Laser.Fade", LaserType.Fade);

		LaserType.Duration = reader->Get("Laser.Duration", LaserType.Duration);
		LaserType.Thickness = reader->Get("Laser.Thickness", LaserType.Thickness);

		std::vector<ColorStruct> colorList{};
		for (int i = 0; i < 128; i++)
		{
			std::string key = "Laser.Color" + std::to_string(i);
			ColorStruct color = reader->Get<ColorStruct>(key, Colors::Empty);
			if (color != Colors::Empty)
			{
				colorList.push_back(color);
			}
		}
		if (!colorList.empty())
		{
			LaserType.ColorList = colorList;
		}
		LaserType.ColorListRandom = reader->Get("Laser.ColorListRandom", LaserType.ColorListRandom);

		// 电弧尾巴
		BoltType.IsAlternateColor = reader->Get("Electric.IsAlternateColor", BoltType.IsAlternateColor);

		BoltType.ArcCount = reader->Get("Bolt.Arcs", BoltType.ArcCount);

		BoltType.Color1 = reader->Get("Bolt.Color1", BoltType.Color1);
		BoltType.Color2 = reader->Get("Bolt.Color2", BoltType.Color2);
		BoltType.Color3 = reader->Get("Bolt.Color3", BoltType.Color3);

		BoltType.Disable1 = reader->Get("Bolt.Disable1", BoltType.Disable1);
		BoltType.Disable2 = reader->Get("Bolt.Disable2", BoltType.Disable2);
		BoltType.Disable3 = reader->Get("Bolt.Disable3", BoltType.Disable3);
		BoltType.DisableParticle = reader->Get("Bolt.DisableParticle", BoltType.DisableParticle);

		// 辐射尾巴
		BeamType.Color = reader->Get("Beam.Color", BeamType.Color);
		BeamType.Period = reader->Get("Beam.Period", BeamType.Period);
		BeamType.Amplitude = reader->Get("Beam.Amplitude", BeamType.Amplitude);

		// 粒子尾巴
		ParticleSystem = reader->Get("ParticleSystem", ParticleSystem);

		// 动画尾巴
		StartDrivingAnim = reader->GetList("Anim.Start", StartDrivingAnim);
		WhileDrivingAnim = reader->GetList("Anim.While", WhileDrivingAnim);
		StopDrivingAnim = reader->GetList("Anim.Stop", StopDrivingAnim);
	}

};
