#pragma once

#include <Common/INI/INI.h>
#include <Common/INI/INIConfig.h>
#include <Common/INI/INIReader.h>

enum class SubjectToGroundType
{
	AUTO = 0,
	YES = 1,
	NO = 2
};

template <>
inline bool Parser<SubjectToGroundType>::TryParse(const char *pValue, SubjectToGroundType *outValue)
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
	virtual void Read(INIBufferReader *ini) override
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

	bool AdvancedBallistics = true;

	// Arcing
	// [INIField(Key = "Arcing.FixedSpeed")]
	int ArcingFixedSpeed = 0;
	bool Inaccurate = false;
	// [INIField(Key = "BallisticScatter.Min")]
	float BallisticScatterMin = 0;
	// [INIField(Key = "BallisticScatter.Max")]
	float BallisticScatterMax = 0;
	int Gravity = RulesClass::Instance->Gravity;

	// Straight
	bool Straight = false;
	bool AbsolutelyStraight = false;

	// Missile
	// [INIField(Key = "ROT.Reverse")]
	bool ReverseVelocity = false;
	// [INIField(Key = "ROT.ReverseZ")]
	bool ReverseVelocityZ = false;
	// [INIField(Key = "ROT.ShakeMultiplier")]
	float ShakeVelocity = 0;

	// Status
	SubjectToGroundType SubjectToGround = SubjectToGroundType::AUTO;

	bool IsStraight()
	{
		return Straight || AbsolutelyStraight;
	}
};
