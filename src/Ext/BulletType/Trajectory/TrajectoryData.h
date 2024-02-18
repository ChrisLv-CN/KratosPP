#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

enum class SubjectToGroundType
{
	AUTO = 0,
	YES = 1,
	NO = 2
};

template <>
inline bool Parser<SubjectToGroundType>::TryParse(const char* pValue, SubjectToGroundType* outValue)
{
	switch (toupper(static_cast<unsigned char>(*pValue)))
	{
	case '1':
	case 'T':
	case 'Y':
		if (outValue)
		{
			*outValue = SubjectToGroundType::YES;
		}
		return true;
	case '0':
	case 'F':
	case 'N':
		if (outValue)
		{
			*outValue = SubjectToGroundType::NO;
		}
		return true;
	}
	return false;
}

class TrajectoryData : public INIConfig
{
public:
	bool AdvancedBallistics = true;

	// Arcing
	int ArcingFixedSpeed = 0; // 恒定速度飞行，近距离高抛，远距离平抛
	bool Inaccurate = false; // 不精确散布
	float BallisticScatterMin = 0; // 最小散布距离
	float BallisticScatterMax = 0; // 最大散布距离
	int Gravity = RulesClass::Instance->Gravity; // 自定义重力

	// Straight
	bool Straight = false; // 直线飞行
	bool AbsolutelyStraight = false; // 朝向正面的直线飞行

	// Missile
	bool ReverseVelocity = false; // 反转出膛飞行方向
	bool ReverseVelocityZ = false; // 反转出膛飞行方向
	float ShakeVelocity = 0; // 出膛方向随机抖动

	// Status
	SubjectToGroundType SubjectToGround = SubjectToGroundType::AUTO;

	virtual void Read(INIBufferReader* ini) override
	{
		this->AdvancedBallistics = ini->Get("AdvancedBallistics", AdvancedBallistics);

		// Arcing
		this->ArcingFixedSpeed = ini->Get("Arcing.FixedSpeed", ArcingFixedSpeed);
		this->Inaccurate = ini->Get("Inaccurate", Inaccurate);
		this->BallisticScatterMin = ini->Get("BallisticScatter.Min", BallisticScatterMin);
		this->BallisticScatterMax = ini->Get("BallisticScatter.Max", BallisticScatterMax);
		this->Gravity = ini->Get("Gravity", Gravity);

		// Straight
		this->Straight = ini->Get("Straight", Straight);
		this->AbsolutelyStraight = ini->Get("AbsolutelyStraight", AbsolutelyStraight);

		// Missile
		this->ReverseVelocity = ini->Get("ROT.Reverse", ReverseVelocity);
		this->ReverseVelocityZ = ini->Get("ROT.ReverseZ", ReverseVelocityZ);
		this->ShakeVelocity = ini->Get("ROT.ShakeMultiplier", ShakeVelocity);

		// Status
		this->SubjectToGround = ini->Get("SubjectToGround", SubjectToGround);
	}

	bool IsStraight()
	{
		return Straight || AbsolutelyStraight;
	}
};
