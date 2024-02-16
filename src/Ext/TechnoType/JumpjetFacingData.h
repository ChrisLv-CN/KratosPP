#pragma once

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>

class JumpjetFacingData : public INIConfig
{
public:
	JumpjetFacingData() : INIConfig()
	{
		Enable = true;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Enable = reader->Get("JumpjetFacingToTarget", Enable);
	}
};
