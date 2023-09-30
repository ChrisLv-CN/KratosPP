#pragma once

#include <Common/INI/INIConfig.h>

class ExpireAnimData : public INIConfig
{
public:
	std::string ExpireAnimOnWater{ "" };

	virtual void Read(INIBufferReader* reader) override
	{
		ExpireAnimOnWater = reader->Get("ExpireAnimOnWater", ExpireAnimOnWater);
	}

};

