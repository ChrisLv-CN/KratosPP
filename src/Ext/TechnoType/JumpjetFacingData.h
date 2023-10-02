#pragma once
#include <string>
#include <vector>

#include <GeneralStructures.h>

#include <Common/INI/INIConfig.h>
#include <Ext/Helper/CastEx.h>

class JumpjetFacingData : public INIConfig
{
public:
	JumpjetFacingData()
	{
		Enable = true;
	}

	virtual void Read(INIBufferReader* reader) override
	{
		Enable = reader->Get("JumpjetFacingToTarget", Enable);
	}
};
