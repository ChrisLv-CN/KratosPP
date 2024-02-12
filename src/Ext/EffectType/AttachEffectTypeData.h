#pragma once

#include <string>
#include <vector>

#include <Common/INI/INIConfig.h>

class AttachEffectTypeData : public INIConfig
{
public:
	std::vector<std::string> AttachEffectTypes{};

	std::vector<double> AttachEffectChances{}; // 附加成功率，应该只对弹头有用
	bool AttachFullAirspace = false; // 搜索圆柱体范围

	// 单条AE
	int StandTrainCabinLength = 512; // 火车替身间隔
	int AEMode = -1; // 作为乘客时的激活载具的组序号

	//多组AE
	bool AttachByPassenger = true; // 仅由乘客的AEMode赋予
	int AEModeIndex = -1; // 组序号

	virtual void Read(INIBufferReader* reader) override
	{
		AttachEffectTypes = reader->GetList("AttachEffectTypes", AttachEffectTypes);

		AttachEffectChances = reader->GetChanceList("AttachEffectChances", AttachEffectChances);
		AttachFullAirspace = reader->Get("AttachFullAirspace", AttachFullAirspace);

		StandTrainCabinLength = reader->Get("StandTrainCabinLength", StandTrainCabinLength);
		// 由乘客读取
		AEMode = reader->Get("AEMode", AEMode);

		Enable = !AttachEffectTypes.empty();
	}

	// 多组AE
	void Read(INIBufferReader* reader, int index)
	{
		std::string title = "AttachEffectTypes" + std::to_string(index);

		AttachEffectTypes = reader->GetList(title, AttachEffectTypes);

		AttachEffectChances = reader->GetChanceList(title + ".Chances", AttachEffectChances);
		AttachByPassenger = reader->Get(title + ".AttachByPassenger", AttachByPassenger);
		AEModeIndex = index;
	}

};
