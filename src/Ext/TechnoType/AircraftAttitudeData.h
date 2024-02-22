#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

#include <Ext/Common/CommonStatus.h>

class AircraftAttitudeData : public INIConfig
{
public:

	bool Disable = false; // 关闭俯仰姿态自动调整，但不影响俯冲

	int SpawnTakeoffDir = 0;
	int SpawnLandDir = 0;


	virtual void Read(INIBufferReader* reader) override
	{
		// 读取全局
		INIBufferReader* av = INI::GetSection(INI::Rules, INI::SectionAudioVisual);
		Read(av, "");
		// 读取个体
		Read(reader, "");
	}

	virtual void Read(INIBufferReader* reader, std::string title)
	{
		Disable = reader->Get("DisableAircraftAutoPitch", Disable);

		SpawnTakeoffDir = reader->Get("SpawnTakingOffDir", SpawnTakeoffDir);
		SpawnLandDir = reader->Get("SpawnLandingDir", SpawnTakeoffDir);

		Enable = !Disable;
	}
};

class AircraftDockingOffsetData : public INIConfig
{
public:
	std::vector<int> Direction{};

	virtual void Read(INIBufferReader* reader) override
	{
		// 读取全局
		int poseDir = RulesClass::Instance->PoseDir;
		std::vector<int> dirs;
		for (int i = 0; i < 128; i++)
		{
			std::string key = "DockingOffset" + std::to_string(i) + ".Dir";
			int dir = reader->Get(key, poseDir);
			if (dir < 0)
			{
				dir = 0;
			}
			else if (dir > 7)
			{
				dir = 7;
			}
			dirs.push_back(dir);
		}
		if (!dirs.empty())
		{
			Direction = dirs;
		}
	}

};
